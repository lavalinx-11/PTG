#pragma once
#include "Triangle.h"
#include <glew.h>
#include <vector>
#include <Vector.h>
#include "Engine/TerrainMeshData.h"
using namespace MATH;


class Mesh {
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) = delete;
	Mesh& operator = (const Mesh&) = delete;
	Mesh& operator = (Mesh&&) = delete;

private:
	const char* filename;
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvCoords;

	size_t indexCount;
	size_t dateLength;
	GLenum drawmode;

	/// Private helper methods
	void LoadModel(const char* filename);
	void StoreMeshData(GLenum drawmode_);
	GLuint vao, vbo, ebo;
public:
	
	Mesh(const char* filename_);
	~Mesh();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;
	void Render(GLenum drawmode) const;

	// Umer messing with Scott’s Mesh class
	// We need the option to create a mesh from a Triangle object
	//default constructor
	Mesh() :
		dateLength{ 0 }
		, drawmode{ 0 }
		, vao{ 0 }
		, vbo{ 0 }
		, ebo{ 0 }
		, filename{ nullptr }
		, indexCount{ 0 }
	{}
	//loads a triangle into the mesh and fills in the vertices, normals, and uvCoords 
	void LoadTriangle(const MATHEX::Triangle* triangle)
	{
		vertices.clear();
		normals.clear();
		uvCoords.clear();
		vertices.push_back(triangle->getV0());
		vertices.push_back(triangle->getV1());
		vertices.push_back(triangle->getV2());
		// Normals and UVs are empty stubs for now
		for (int i = 0; i < 3; i++) {
			normals.push_back(Vec3());
			uvCoords.push_back(Vec2());
		}
	}

	bool OnCreate(const MATHEX::Triangle* triangle)
	{
		LoadTriangle(triangle);
		StoreMeshData(GL_TRIANGLES);
		return true;
	}


	// Terrain Mesh Data version of OnCreate
	bool OnCreate(const TerrainMeshData& data);
};

