#pragma once
#ifndef SCENE3G_H
#define SCENE3G_H
#include "Scene.h"
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

class Scene3g : public Scene {
private:
	Body* sub;
	Shader* reflectionShader;
	Mesh* mesh;
	Texture* texture;
	Matrix4 modelMatrix;
	bool drawInWireMode;
	Vec4 Diffuse[5];
	Vec4 Specular[5];
	Vec3 Litpos[5];
	Camera* cam;

public:
	explicit Scene3g();
	virtual ~Scene3g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;
};


#endif // SCENE3G_H