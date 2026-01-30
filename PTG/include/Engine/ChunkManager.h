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


	// <- Chunk parameters and storage -> //
	int chunkResolution = 50; // Default is 50
	float chunkSize = 40.0f; // Default is 40.0f
	int seed = 1337; // Default seed
	int viewRadius = 10; // Default view radius 10
	std::unordered_map<long long, TerrainChunk*> chunks;
	int visibleChunkCount = 0;

	// <- Debug options -> //
	bool showChunkDebug = false;
	bool showAsyncDebug = true;

	// <- Worker thread and job queues ->//
	std::thread workerThread;
	std::mutex jobMutex;
	std::queue<ChunkBuildTask> jobQueue;	
	std::condition_variable jobCv;
	std::mutex completedMutex;
	std::queue<ChunkBuildResult> completedQueue;

	// <- Atomic running flag -> //
	std::atomic<bool> running = true;



	// black



	//  <- Assistant functions -> //
	long long ChunkKey(int chunkX, int chunkZ) const {
		return (static_cast<long long>(chunkX) << 32) | (static_cast<unsigned int>(chunkZ) & 0xFFFFFFFF);
	}

	int WorldToChunk(float worldPos) {
		return static_cast<int>(std::floor(worldPos / chunkSize));
	}

	static Vec3 ColourForChunkState(TerrainChunk::ChunkState s) {
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
	
	// <- Main update and render functions -> //
	void Update(const Camera& cam) {
		int camChunkX = WorldToChunk(cam.GetPosition().x);
		int camChunkZ = WorldToChunk(cam.GetPosition().z);

		StreamChunks(camChunkX, camChunkZ);
		RemoveFarChunks(camChunkX, camChunkZ);
		UpdateLODs(cam.GetPosition());

		const int uploadBudget = 60; 
		int uploadsThisFrame = 0;


		// ;; for infinite loop until all jobs completed or budget used up
		for (;;) {
			if (uploadsThisFrame >= uploadBudget)
				break;

			ChunkBuildResult result;

			{
				// Check for completed jobs
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

			// Apply the result to the chunk
			TerrainChunk* chunk = it->second;
			chunk->SetState(TerrainChunk::ChunkState::Ready);
			chunk->CreateMeshFromData(result.data);
			chunk->SetState(TerrainChunk::ChunkState::Uploaded);
			uploadsThisFrame++;
		}

	}

	void Render(const Camera& cam, Shader& shader) {
		visibleChunkCount = 0;

		// Count chunks by state for debug output
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

		// Render each chunk with debug coloring
		for (const auto& pair : chunks) {
			TerrainChunk* chunk = pair.second;

			bool visible = cam.IsAABBVisible(chunk->GetBounds());
			
			Vec3 colour;

			if (showAsyncDebug) {
				// Colour by async state
				colour = ColourForChunkState(chunk->GetState());

				// If you still want culled chunks obvious, override to red:
				if (!visible) {
					colour = Vec3(1.0f, 0.0f, 0.0f);
				}
			}
			else {
				// Colour by frustum visibility
				colour = visible ? Vec3(0.0f, 1.0f, 0.0f)
					: Vec3(1.0f, 0.0f, 0.0f);
			}


			glUniform3fv(uDebugColor, 1, &colour.x);

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


	// <- Chunk management functions -> //
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

	void ToggleAsyncDebug() { showAsyncDebug = !showAsyncDebug; }


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

	// <- Worker thread function -> //
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




		
};

