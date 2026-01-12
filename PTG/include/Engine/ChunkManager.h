#pragma once
#include "Engine/TerrainChunk.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
#include <unordered_map>
#include <cmath>

class ChunkManager {
private:
	int chunkResolution = 50; // Default is 50
	float chunkSize = 40.0f; // Default is 40.0f
	int seed = 1337; // Default seed
	int viewRadius = 10; // Default view radius 10
	std::unordered_map<long long, TerrainChunk*> chunks;
	int visibleChunkCount = 0;
	bool showChunkDebug = false;

	long long ChunkKey(int chunkX, int chunkZ) const {
		return (static_cast<long long>(chunkX) << 32) | (static_cast<unsigned int>(chunkZ) & 0xFFFFFFFF);
	}

	int WorldToChunk(float worldPos) {
		return static_cast<int>(std::floor(worldPos / chunkSize));
	}

public:
	ChunkManager(int chunkResolution_, float chunkSize_, int seed_, int viewRadius_)
		: chunkResolution(chunkResolution_), chunkSize(chunkSize_), seed(seed_), viewRadius(viewRadius_) {}

	~ChunkManager() {
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
	}

	void Render(const Camera& cam, Shader& shader) {
		visibleChunkCount = 0;

		GLuint uDebugColor = shader.GetUniformID("debugColor");

		for (const auto& pair : chunks) {
			TerrainChunk* chunk = pair.second;

			bool visible = cam.IsAABBVisible(chunk->GetBounds());
			
			Vec3 color = visible
				? Vec3(0.0f, 1.0f, 0.0f)
				: Vec3(1.0f, 0.0f, 0.0f);

			glUniform3fv(uDebugColor, 1, &color.x);

			if (visible) {
				chunk->Render(shader);
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
					chunks[key] = new TerrainChunk(
						x,
						z,
						chunkResolution,
						chunkSize,
						seed
					);
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
};

