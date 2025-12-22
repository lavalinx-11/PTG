#pragma once
#ifndef PTGSCENE_H
#define PTGSCENE_H
#include "Scenes/Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include "Graphics/Camera.h"
#include "Engine/ChunkManager.h"

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


	ChunkManager* chunkManager;

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
};


#endif // PTGSCENE_H