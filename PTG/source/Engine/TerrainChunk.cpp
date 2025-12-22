#include "Engine/TerrainChunk.h"
#include "Engine/TerrainMeshBuilder.h"

TerrainChunk::TerrainChunk(
    int x,
    int z,
    int resolution_,
    float size_,
    int seed_
)
    : chunkX(x)
    , chunkZ(z)
    , resolution(resolution_)
    , chunkSize(size_)
    , seed(seed_)
{
    TerrainMeshData data =
        TerrainMeshBuilder::BuildFlatGrid(
            resolution,
            chunkSize,
            chunkX,
            chunkZ,
            seed
        );
    float minX = chunkX * chunkSize;
    float minZ = chunkZ * chunkSize;
    float maxX = minX + chunkSize;
    float maxZ = minZ + chunkSize;

    float minY = -50.0f;
    float maxY = 50.0f;

    bounds.min = Vec3(minX, minY, minZ);
    bounds.max = Vec3(maxX, maxY, maxZ);
    mesh = new Mesh();
    mesh->OnCreate(data);
}

TerrainChunk::~TerrainChunk() {
    if (mesh) {
        mesh->OnDestroy();
        delete mesh;
    }
}


void TerrainChunk::Render(Shader& shader) const {
    Vec3 color;

    switch (currentLOD) {
    case 2: color = Vec3(1.0f, 0.0f, 0.0f); break; // LOW
    case 1: color = Vec3(0.0f, 1.0f, 0.0f); break; // MEDIUM 
    case 0: color = Vec3(0.0f, 0.5f, 1.0f); break; // HIGH 
    default: color = Vec3(1.0f, 1.0f, 1.0f); break;
    }

    glUniform3f(shader.GetUniformID("debugColor"),
        color.x, color.y, color.z
    );

    mesh->Render(GL_TRIANGLES);
}

float TerrainChunk::DistanceToCamera(const Vec3& camPos) const
{
    float centerX = (chunkX + 0.5f) * chunkSize;
    float centerZ = (chunkZ + 0.5f) * chunkSize;

    float dx = camPos.x - centerX;
    float dz = camPos.z - centerZ;

    return std::sqrt(dx * dx + dz * dz);
}

int TerrainChunk::ComputeLOD(const Vec3& camPos) const
{
    float dist = DistanceToCamera(camPos);

    if (dist < 50.0f) return 0; // HIGH
    if (dist < 120.0f) return 1; // MEDIUM
    return 2; // LOW
}

void TerrainChunk::UpdateLOD(const Vec3& camPos) {
    int newLOD = ComputeLOD(camPos);

    if (newLOD == currentLOD)
        return;

    currentLOD = newLOD;
    resolution = LOD_RESOLUTIONS[currentLOD];

    TerrainMeshData data = TerrainMeshBuilder::BuildFlatGrid(
            resolution,
            chunkSize,
            chunkX,
            chunkZ,
            seed
        );

    mesh->OnDestroy();
    mesh->OnCreate(data);
}
