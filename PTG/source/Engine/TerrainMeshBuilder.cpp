#include "Engine/TerrainMeshBuilder.h"



// Helper functions
namespace {
    inline int Index(int x, int z, int vertexCount) {
        return z * vertexCount + x;
    }

    static void RecalculateNormals(TerrainMeshData& data, int vertexCount) {
        for (int z = 0; z < vertexCount; z++) {
            for (int x = 0; x < vertexCount; x++) {

                int idx = Index(x, z, vertexCount);

                int xL = std::max(x - 1, 0);
                int xR = std::min(x + 1, vertexCount - 1);
                int zD = std::max(z - 1, 0);
                int zU = std::min(z + 1, vertexCount - 1);

                float hL = data.vertices[Index(xL, z, vertexCount)].y;
                float hR = data.vertices[Index(xR, z, vertexCount)].y;
                float hD = data.vertices[Index(x, zD, vertexCount)].y;
                float hU = data.vertices[Index(x, zU, vertexCount)].y;

                Vec3 n(hL - hR, 2.0f, hD - hU);
                data.normals[idx] = VMath::normalize(n);
            }
        }
    }
}

// Terrain Builder API
TerrainMeshData TerrainMeshBuilder::BuildFlatGrid(int resolution, float scale, int chunkX, int chunkZ, int seed)
{
    TerrainMeshData meshData;
    NoiseGenerator noise(seed);
    
	// Each terrain grid will have the resolution + 1 vertices along each side
    int vertexCount = resolution + 1;
    int totalVertices = vertexCount * vertexCount;
    int totalIndices = resolution * resolution * 6;
    float chunkWorldOffsetX = chunkX * scale;
    float chunkWorldOffsetZ = chunkZ * scale;

	// Pre-allocating space in vectors
    meshData.vertices.reserve(totalVertices);
    meshData.normals.resize(totalVertices);
    meshData.uvs.reserve(totalVertices);
    meshData.indices.reserve(totalIndices);

    // Generate vertices, normals, and UVs
    for (int z = 0; z < vertexCount; z++) {
        for (int x = 0; x < vertexCount; x++) {

			// Normalized coordinates
            float fx = static_cast<float>(x) / resolution;
            float fz = static_cast<float>(z) / resolution;

			// Mapping to world coordinates
            float localX = fx * scale;
            float localZ = fz * scale;

            float worldX = localX + chunkWorldOffsetX;
            float worldZ = localZ + chunkWorldOffsetZ;
			// Height from noise function
            float height = noise.GetHeight(worldX, worldZ);

			// Settting the vertex position and UV coordinates
            meshData.vertices.emplace_back(worldX, height, worldZ);
           
            meshData.uvs.emplace_back(fx, fz);

        }
    }
	// Generate normals
            meshData.normals.resize(meshData.vertices.size());
			RecalculateNormals(meshData, vertexCount);
			// Generate indices for triangles
    for (int z = 0; z < resolution; z++) {
        for (int x = 0; x < resolution; x++) {

            int topLeft = Index(x, z, vertexCount);
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * vertexCount + x;
            int bottomRight = bottomLeft + 1;

            // Triangle 1
            meshData.indices.push_back(topLeft);
            meshData.indices.push_back(bottomLeft);
            meshData.indices.push_back(topRight);

            // Triangle 2
            meshData.indices.push_back(topRight);
            meshData.indices.push_back(bottomLeft);
            meshData.indices.push_back(bottomRight);
        }
    }
    return meshData;
}


// Get height at grid coordinate (x, z)
float TerrainMeshBuilder::GetHeightAt(const TerrainMeshData& data, int x, int z, int vertexCount)
{
	int index = z * vertexCount + x;
    return data.vertices[index].y;
}



