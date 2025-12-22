#pragma once
#include "Engine/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
struct AABB {
    Vec3 min;
    Vec3 max;
};

class TerrainChunk {
private:
    int chunkX;
    int chunkZ;
	int resolution;
    int seed;
	int currentLOD = -1;
	float chunkSize;
    Mesh* mesh = nullptr;
    AABB bounds;
    static constexpr int LOD_RESOLUTIONS[3] = {
       64, // HIGH
       32, // MEDIUM
       16  // LOW
    };

    enum class ChunkState {
        UNLOADED,
        LOADING,
        LOADED,
        UPLOADED
	};;
public:
    TerrainChunk(
        int chunkX,
        int chunkZ,
        int resolution,
        float size,
        int seed
    );

    ~TerrainChunk();
	void UpdateLOD(const Vec3& cameraPosition);
    void Render(Shader& shader) const;


    int GetChunkX() const { return chunkX; }
    int GetChunkZ() const { return chunkZ; }

    
    float DistanceToCamera(const Vec3& camPos) const;
	int ComputeLOD(const Vec3& camPos) const;
    const AABB& GetBounds() const { return bounds; }
};
