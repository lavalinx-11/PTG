#pragma once
#include <vector>
#include <Vector.h>
#include <VMath.h>

using namespace MATH;

struct TerrainMeshData {
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;
	std::vector<unsigned int> indices;
	int seed;

	void Clear() {
		vertices.clear();
		normals.clear();
		uvs.clear();
		indices.clear();
	}
	bool IsValid() const {
		return !vertices.empty() &&
			vertices.size() == normals.size() &&
			vertices.size() == uvs.size() &&
			!indices.empty();
	}
	size_t VertexCount() const { return vertices.size(); }
	size_t IndexCount()  const { return indices.size(); }
};