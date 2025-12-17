#ifndef SCENE3P_H
#define SCENE3P_H
#include <iostream>
#include "Scene.h"
#include "Vector.h"
#include <Vector>
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

class Scene3p : public Scene {
private:
	// We will have one big sphere for the jellyfish's head
	Body* jellyfishHead;
	Matrix4 jellyMatrix;

	// Smaller spheres just to show us where the tentacles are anchored
	std::vector<Body*> anchors;
	std::vector<Matrix4> achorMatrix;

	// And the smallest spheres will make up the tentacles
	std::vector<Body*> tentacleSpheres;
	std::vector<Matrix4> tentacleMatrix;

	// These variables will tells us how long each tentacle will be
	// and the spacing between individual spheres
	const int numSpheresPerAnchor = 100;
	const float spacing = 1.0f;
	const int numAnchors = 10;

	Vec3	   cameraPos;
	Quaternion cameraOri;

	Mesh* mesh;
	SkyBox* skyblox;

	Shader* shader;
	Shader* reflectionShader;
	Shader* drawNormalsShader;


	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;

	

	Camera* cam;



	int anchorIndex = 0;
	int tentacleIndex = 0;
	bool rolling = false;
	bool drawInWireMode;
	bool drawNormals;
	float planeAngleRadians;

public:
	explicit Scene3p();
	virtual ~Scene3p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;

	void DrawNormals(const Vec4 color) const;
};


#endif // SCENE3P_H