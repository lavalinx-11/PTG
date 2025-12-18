#pragma once
#ifndef PTGSCENE_H
#define PTGSCENE_H
#include "Scenes/Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include "Graphics/Camera.h"
#include "Engine/TerrainMeshBuilder.h"
#include "Engine/TerrainChunk.h"
#include <unordered_map>

using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class Texture;

class PTGScene : public Scene {
private:
	// Body

	// Shaders
	Shader* terrainShader;

	// Meshes
	Mesh* mesh;
	


	// Camera
	Camera* cam;

	// Textures
	/*Texture* terrainTexture;
	Texture* heightMap;
	Texture* diffuseMap;
	Texture* normalMap;
	Texture* texture;*/


	// Terrain Generation Variables
	int chunkResolution = 32;
	float chunkSize = 40.0f;
	int seed = 1337;
	int viewRadius = 10;
	std::unordered_map<long long, TerrainChunk*> chunks;


	// Math Variables
	Matrix4 modelMatrix;
	Vec3	   cameraPos;
	Quaternion cameraOri;
	// Regular Variables
	bool drawInWireMode;

public:
	explicit PTGScene();
	virtual ~PTGScene();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
	inline long long ChunkKey(int x, int z) {
		return (static_cast<long long>(x) << 32) ^ (z & 0xffffffff);
	}
};


#endif // PTGSCENE_H