#pragma once
#include "Engine/Mesh.h"
#include "Graphics/Shader.h"

class TerrainChunk {
private:
    int chunkX;
    int chunkZ;
    Mesh* mesh;


public:
    TerrainChunk(
        int chunkX,
        int chunkZ,
        int resolution,
        float size,
        int seed
    );

    ~TerrainChunk();

    void Render(const Shader& shader) const;

    // Move these methods to public to fix accessibility error
    int GetChunkX() const { return chunkX; }
    int GetChunkZ() const { return chunkZ; }


};
