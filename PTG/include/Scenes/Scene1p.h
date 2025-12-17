#ifndef SCENE1P_H
#define SCENE1P_H
#include "Scenes/Scene.h"
#include "Vector.h"
#include <Matrix.h>
#include <QMath.h>
#include "Engine/Plane.h"
#include "Graphics/Camera.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class Scene1p : public Scene {
private:


	Body* plane;
	Body* sphere;
	Mesh* mesh;
	Mesh* planeMesh;
	Shader* shader;
	Plane* planeShape;
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
	Camera *cam;
	bool drawInWireMode;

public:
	explicit Scene1p();
	virtual ~Scene1p();

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime) override;
	virtual void Render() const override;
	virtual void HandleEvents(const SDL_Event &sdlEvent) override;
};


#endif // SCENE1P_H