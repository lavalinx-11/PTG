#pragma once
#ifndef SCENE4G_H
#define SCENE4G_H
#include "Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include "Trackball.h"
#include "Camera.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class Texture;

class Scene4g : public Scene {
private:
	// Body
	Body* sub;
	Body* terrain;


	// Shaders
	Shader* tessShader;
	Shader* reflectionShader;
	
	// Meshes
	Mesh* mesh;
	Mesh* terrainMesh;
	
	// Trackball and Camera
	Trackball trackball;
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
	
	// Regular Variables
	bool drawInWireMode;
	float tessLevel = 1.0f;

public:
	explicit Scene4g();
	virtual ~Scene4g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE4G_H