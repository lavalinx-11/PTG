#ifndef SCENE2P_H
#define SCENE2P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <QMath.h>
#include "Graphics/Camera.h"
#include "Engine/Plane.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class Texture;

class Scene2p : public Scene {
private:
	Body* sphereA;
	Body* sphereB;
	Body* plane;


	Mesh* sphereAmesh;
	Mesh* sphereBmesh;
	Mesh* planeMesh;

	Vec3 planeNormal;

	Plane* planeShape;

	SkyBox* skyblox;

	Shader* shader;
	Shader* drawNormalsShader;


	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;

	Texture* sphereATex;
	Texture* sphereBTex;
	Texture* planeTex;

	Camera* cam;

	Vec4 Diffuse[5];
	Vec4 Specular[5];
	Vec3 Litpos[5];



	bool rolling = false;
	bool drawInWireMode;
	bool drawNormals;
	float planeAngleRadians;

public:
	explicit Scene2p();
	virtual ~Scene2p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;

	void DrawNormals(const Vec4 color) const;
};


#endif // SCENE2P_H