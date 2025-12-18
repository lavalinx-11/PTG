#pragma once
#include "Engine/TerrainMeshData.h"
#include "Engine/NoiseGenerator.h"
class TerrainMeshBuilder {
public:
	static TerrainMeshData BuildFlatGrid(
		int resolution, // number of squares along one side
		float scale, // size of the terrain in world units
		int chunkX,
		int chunkZ,
		int seed // seed for noise generation
	);

	static float GetHeightAt(const TerrainMeshData& data, int x, int z, int vertexCount);


};