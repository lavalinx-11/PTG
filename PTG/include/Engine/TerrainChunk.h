#pragma once
#include "Engine/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Camera.h"
struct AABB {
    Vec3 min;
    Vec3 max;
};

class TerrainChunk {
public:
    enum class ChunkState {
        Unloaded,
        Generating,
        Ready,
        Uploaded
    };

private:
    static constexpr int LOD_RESOLUTIONS[3] = {
       128, // HIGH
       16, // MEDIUM
       8  // LOW
    };


    int chunkX;
    int chunkZ;
    int resolution;
    int seed;
    int currentLOD = -1;
    float chunkSize;
    Mesh* mesh = nullptr;
    AABB bounds;
    ChunkState state = ChunkState::Unloaded;
    TerrainMeshData pendingMeshData;


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

    void SetState(ChunkState newState) {
        state = newState;
    }

    ChunkState GetState() const {
        return state;
    }

    void SetPendingMeshData(const TerrainMeshData& data) { pendingMeshData = data; }
    TerrainMeshData GetPendingMeshData() const { return pendingMeshData; }
    void ClearMeshData() {
        pendingMeshData.Clear();
    }

    void CreateMeshFromData(const TerrainMeshData& data) {
        if (!mesh)
            mesh = new Mesh();

        mesh->OnCreate(data);
    }


    float DistanceToCamera(const Vec3& camPos) const;
    int ComputeLOD(const Vec3& camPos) const;
    const AABB& GetBounds() const { return bounds; }
    bool HasMesh() const {
        return mesh != nullptr;
    }
};
