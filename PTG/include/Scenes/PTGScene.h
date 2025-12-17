#pragma once
#ifndef PTGSCENE_H
#define PTGSCENE_H
#include "Scenes/Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include "Graphics/Camera.h"
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
	Body* terrain;


	// Shaders
	Shader* tessShader;
	Shader* reflectionShader;
	
	// Meshes
	Mesh* mesh;
	Mesh* terrainMesh;
	
	// Camera
	Camera* cam;

	// Textures
	Texture* terrainTexture;
	Texture* heightMap;
	Texture* diffuseMap;
	Texture* normalMap;
	Texture* texture;
	
	
	// Math Variables
	Matrix4 terrainModelMatrix;
	Matrix4 modelMatrix;
	Vec4 Diffuse[5];
	Vec4 Specular[5];
	Vec3 Litpos;
	Vec3	   cameraPos;
	Quaternion cameraOri;
	// Regular Variables
	bool drawInWireMode;
	float tessLevel = 1.0f;

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