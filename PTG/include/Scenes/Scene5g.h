#ifndef SCENE5G_H
#define SCENE5G_H
#include <iostream>
#include "Scenes/Scene.h"
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

class Scene5g : public Scene {
private:
	// Body
	Body* sub;
	Body* terrain;
	Body* jellyfishHead;
	std::vector<Body*> anchors;
	std::vector<Body*> tentacleSpheres;
	const int numSpheresPerAnchor = 100;
	const float spacing = 1.0f;
	const int numAnchors = 10;



	// Meshes
	Mesh* mesh;
	Mesh* terrainMesh;

	SkyBox* skyblox;

	// Shaders
	Shader* tessShader;
	Shader* reflectionShader;
	Shader* shader;
	Shader* drawNormalsShader;

	// Textures
	Texture* terrainTexture;
	Texture* heightMap;
	Texture* diffuseMap;
	Texture* normalMap;
	Texture* texture;

	// Camera
	Camera* cam;



	// Math Variables
	Vec3	   cameraPos;
	Vec3 Litpos;
	Vec4 Diffuse[5];
	Vec4 Specular[5];
	Matrix4 terrainModelMatrix;
	Matrix4 modelMatrix;
	Matrix4 jellyMatrix;
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
	std::vector<Matrix4> achorMatrix;
	std::vector<Matrix4> tentacleMatrix;
	Quaternion cameraOri;

	
	

	// Regular Variables
	int anchorIndex = 0;
	int tentacleIndex = 0;
	bool rolling = false;
	bool drawInWireMode;
	bool drawNormals;
	float tessLevel = 1.0f;
	float planeAngleRadians;

public:
	explicit Scene5g();
	virtual ~Scene5g();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event& sdlEvent) override;

	void DrawNormals(const Vec4 color) const;
};


#endif // SCENE5G_H