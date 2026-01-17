#pragma once
#include "Engine/TerrainChunk.h"
#include "Engine/TerrainMeshBuilder.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include <unordered_map>
#include <cmath>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>


struct ChunkBuildTask {
	int chunkX;
	int chunkZ;
};
struct ChunkBuildResult {
	int chunkX;
	int chunkZ;
	TerrainMeshData data;
};	
class ChunkManager {
private:
	int chunkResolution = 50; // Default is 50
	float chunkSize = 40.0f; // Default is 40.0f
	int seed = 1337; // Default seed
	int viewRadius = 10; // Default view radius 10
	std::unordered_map<long long, TerrainChunk*> chunks;
	int visibleChunkCount = 0;
	bool showChunkDebug = false;
	std::thread workerThread;
	std::mutex jobMutex;
	std::queue<ChunkBuildTask> jobQueue;	
	std::condition_variable jobCv;

	// Completed results (worker -> main thread)
	std::mutex completedMutex;
	std::queue<ChunkBuildResult> completedQueue;

	std::atomic<bool> running = true;
	bool showAsyncDebug = true;

	//  <- Assistant functions -> //
	long long ChunkKey(int chunkX, int chunkZ) const {
		return (static_cast<long long>(chunkX) << 32) | (static_cast<unsigned int>(chunkZ) & 0xFFFFFFFF);
	}

	int WorldToChunk(float worldPos) {
		return static_cast<int>(std::floor(worldPos / chunkSize));
	}

	static Vec3 ColorForChunkState(TerrainChunk::ChunkState s) {
		switch (s) {
		case TerrainChunk::ChunkState::Generating: return Vec3(1.0f, 1.0f, 0.0f); 
		case TerrainChunk::ChunkState::Ready:      return Vec3(0.0f, 0.5f, 1.0f); // blue
		case TerrainChunk::ChunkState::Uploaded:   return Vec3(0.0f, 1.0f, 0.0f); // green
		default:                                   return Vec3(1.0f, 0.0f, 1.0f); // magenta (unknown)
		}
	}


public:
	ChunkManager(int chunkResolution_, float chunkSize_, int seed_, int viewRadius_)
		: chunkResolution(chunkResolution_), 
		chunkSize(chunkSize_), 
		seed(seed_), 
		viewRadius(viewRadius_)
	{
		running = true;
		workerThread = std::thread(&ChunkManager::WorkerLoop, this);

	}

	~ChunkManager() {
		running = false;
		jobCv.notify_all();
		if (workerThread.joinable())
			workerThread.join();

		for (auto& pair : chunks) {
			delete pair.second;
		}
		chunks.clear();
	}
	

	void Update(const Camera& cam) {
		int camChunkX = WorldToChunk(cam.GetPosition().x);
		int camChunkZ = WorldToChunk(cam.GetPosition().z);

		StreamChunks(camChunkX, camChunkZ);
		RemoveFarChunks(camChunkX, camChunkZ);
		UpdateLODs(cam.GetPosition());

		// Apply completed async jobs (MAIN THREAD)
		const int uploadBudget = 2; // keep your budget so you can SEE blue/ready states
		int uploadsThisFrame = 0;

		for (;;) {
			if (uploadsThisFrame >= uploadBudget)
				break;

			ChunkBuildResult result;

			{
				std::lock_guard<std::mutex> lock(completedMutex);
				if (completedQueue.empty())
					break;

				result = std::move(completedQueue.front());
				completedQueue.pop();
			}

			long long key = ChunkKey(result.chunkX, result.chunkZ);
			auto it = chunks.find(key);
			if (it == chunks.end()) {
				// chunk got deleted while worker was generating it
				continue;
			}

			TerrainChunk* chunk = it->second;

			// Mark "Ready" briefly if you want (blue)
			chunk->SetState(TerrainChunk::ChunkState::Ready);

			// Upload GPU mesh (MAIN THREAD)
			chunk->CreateMeshFromData(result.data);
			chunk->SetState(TerrainChunk::ChunkState::Uploaded);

			uploadsThisFrame++;
		}

	}

	void Render(const Camera& cam, Shader& shader) {
		visibleChunkCount = 0;
		int generating = 0, ready = 0, uploaded = 0;

		for (const auto& pair : chunks) {
			auto s = pair.second->GetState();
			if (s == TerrainChunk::ChunkState::Generating) generating++;
			else if (s == TerrainChunk::ChunkState::Ready) ready++;
			else if (s == TerrainChunk::ChunkState::Uploaded) uploaded++;
		}

		if (showChunkDebug) {
			std::cout
				<< "Chunks: gen=" << generating
				<< " ready=" << ready
				<< " uploaded=" << uploaded
				<< " visible=" << visibleChunkCount
				<< std::endl;
		}


		GLuint uDebugColor = shader.GetUniformID("debugColor");

		for (const auto& pair : chunks) {
			TerrainChunk* chunk = pair.second;

			bool visible = cam.IsAABBVisible(chunk->GetBounds());
			
			Vec3 color;

			if (showAsyncDebug) {
				// Color by ASYNC STATE first
				color = ColorForChunkState(chunk->GetState());

				// If you still want culled chunks obvious, override to red:
				if (!visible) {
					color = Vec3(1.0f, 0.0f, 0.0f);
				}
			}
			else {
				// Original: color by frustum visibility
				color = visible ? Vec3(0.0f, 1.0f, 0.0f)
					: Vec3(1.0f, 0.0f, 0.0f);
			}


			glUniform3fv(uDebugColor, 1, &color.x);

			if (visible && chunk->HasMesh()) {
				chunk->Render(shader);
			}

			if (visible) {
				visibleChunkCount++;
			}
		}

		if (showChunkDebug) {
			int visible = ChunksInView(cam);

			std::cout << "Visible chunks: " << visible << std::endl;
		}
	}

	void StreamChunks(int camChunkX, int camChunkZ) {
		for (int z = camChunkZ - viewRadius; z <= camChunkZ + viewRadius; z++) {
			for (int x = camChunkX - viewRadius; x <= camChunkX + viewRadius; x++) {

				long long key = ChunkKey(x, z);

				if (chunks.find(key) == chunks.end()) {
					TerrainChunk* chunk = new TerrainChunk(x, z, chunkResolution, chunkSize, seed);
					chunk->SetState(TerrainChunk::ChunkState::Generating);
					chunks[key] = chunk;

					{
						std::lock_guard<std::mutex> lock(jobMutex);
						jobQueue.push({ x, z });
					}
					jobCv.notify_one();
				}
			}
		}
	}

	void RemoveFarChunks(int camChunkX, int camChunkZ) {
		for (auto it = chunks.begin(); it != chunks.end(); ) {
			TerrainChunk* chunk = it->second;

			int dx = std::abs(chunk->GetChunkX() - camChunkX);
			int dz = std::abs(chunk->GetChunkZ() - camChunkZ);

			if (dx > viewRadius || dz > viewRadius) {
				delete chunk;
				it = chunks.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void UpdateLODs(const Vec3& camPos) {
		for (auto& pair : chunks) {
			pair.second->UpdateLOD(camPos);
		}
	}
	int ChunksInView(const Camera& camera) const {
		int visibleCount = 0;

		for (const auto& pair : chunks) {
			if (camera.IsAABBVisible(pair.second->GetBounds())) {
				visibleCount++;
			}
		}

		return visibleCount;
	}


	int GetVisibleChunkCount() const {
		return visibleChunkCount;
	}
	void ShowChunksInViewToggle() {
		if (showChunkDebug) {
			showChunkDebug = false;
		}
		else
			showChunkDebug = true;
	}


	void WorkerLoop() {
		while (running) {
			ChunkBuildTask job;

			{   // Wait for work (prevents busy-spin)
				std::unique_lock<std::mutex> lock(jobMutex);
				jobCv.wait(lock, [&] { return !running || !jobQueue.empty(); });

				if (!running)
					break;

				job = jobQueue.front();
				jobQueue.pop();
			}

			// Build CPU mesh (NO OpenGL, and DO NOT access chunks[])
			TerrainMeshData data = TerrainMeshBuilder::BuildFlatGrid(
				chunkResolution,
				chunkSize,
				job.chunkX,
				job.chunkZ,
				seed
			);

			std::this_thread::sleep_for(std::chrono::milliseconds(50));

			// Push completed result for main thread to apply
			{
				std::lock_guard<std::mutex> lock(completedMutex);
				completedQueue.push({ job.chunkX, job.chunkZ, std::move(data) });
			}
		}
	}


	void ToggleAsyncDebug() { showAsyncDebug = !showAsyncDebug; }


		
};

