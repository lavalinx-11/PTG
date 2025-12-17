#pragma once
#ifndef SCENE2G_H
#define SCENE2G_H
#include "Scene.h"
#include <Vector.h>
#include <Matrix.h>
#include "Graphics/Trackball.h"
#include "Graphics/Camera.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class Texture;

class Scene2g : public Scene {
private:
	Body* skull;
	Body* leftEye;
	Body* rightEye;
	Mesh* leftEyeMesh;
	Mesh* rightEyeMesh;
	Shader* shader;
	Mesh* mesh;
	Texture* texture;
	Texture* leftEyeTexture;
	Texture* rightEyeTexture;
	Matrix4 modelMatrix;
	bool drawInWireMode;
	Trackball trackball;
	Vec4 Diffuse[5];
	Vec4 Specular[5];
	Vec3 Litpos[5];
	Camera* cam;

public:
	explicit Scene2g();
	virtual ~Scene2g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE2G_H