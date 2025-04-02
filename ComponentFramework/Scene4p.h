#ifndef SCENE4P_H
#define SCENE4P_H
#include "Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <QMath.h>
#include "Trackball.h"
#include "Camera.h"
#include "Plane.h"
#include "Triangle.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;
class Texture;

class Scene4p : public Scene {
private:
	Body* triangleBody;
	Body* sphereA;
	Body* pointOnPlane; 
	Body* pointOnLine01; // Point on line going through vertices v0 and v1
	Body* pointOnLine12; // vertices v1 and v2
	Body* pointOnLine20; // vertices v2 and v0
	Body* collisionPoint;

	
	//Meshes
	Mesh* triangleMesh;
	Mesh* sphereAmesh;
	
	
	MATHEX::Triangle* triangleShape;

	//Textures
	Texture* sphereATex;

	//Skybox
	SkyBox* skyblox;

	//Shaders
	Shader* shader;
	Shader* drawNormalsShader;
	Shader* colourShader;
	//Matrices
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;

	//Trackball
	Trackball trackball;
	
	//Camera
	Camera* cam;


	//Variables

	bool rolling = false;
	bool drawInWireMode;
	bool drawNormals;
	float planeAngleRadians;

public:
	explicit Scene4p();
	virtual ~Scene4p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;

	void DrawNormals(const Vec4 color) const;
};


#endif // SCENE4P_H