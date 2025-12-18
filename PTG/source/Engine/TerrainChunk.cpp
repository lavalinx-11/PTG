#include "Engine/TerrainChunk.h"
#include "Engine/TerrainMeshBuilder.h"

TerrainChunk::TerrainChunk(
    int x,
    int z,
    int resolution,
    float size,
    int seed
)
    : chunkX(x), chunkZ(z)
{
    TerrainMeshData data =
        TerrainMeshBuilder::BuildFlatGrid(
            resolution,
            size,
            chunkX,
            chunkZ,
            seed
        );

    mesh = new Mesh();
    mesh->OnCreate(data);
}

TerrainChunk::~TerrainChunk() {
    mesh->OnDestroy();
    delete mesh;
}

void TerrainChunk::Render(const Shader& shader) const {
    mesh->Render(GL_TRIANGLES);
}