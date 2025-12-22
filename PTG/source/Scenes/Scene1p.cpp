#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Scenes/Scene1p.h"
#include <MMath.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Engine/Body.h"
#include "Engine/Plane.h"
#include "Graphics/Shader.h"


Scene1p::Scene1p()
	: sphere{ nullptr }
	, shader{ nullptr }
	, cam{ nullptr }
	, mesh{ nullptr }
	, drawInWireMode{ true }
	, plane{ nullptr }
	, planeMesh{ nullptr }
{
	Debug::Info("Created Scene1p: ", __FILE__, __LINE__);
}

Scene1p::~Scene1p() {
	Debug::Info("Deleted Scene1p: ", __FILE__, __LINE__);
}

bool Scene1p::OnCreate() {
	Debug::Info("Loading assets Scene1p: ", __FILE__, __LINE__);
	sphere = new Body();
	sphere->OnCreate();
	sphere->rad = 1;
	planeShape = new Plane();
	planeShape->normal = Vec3(0.0f, 0.0f, -1.0f);


	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	plane = new Body();
	plane->OnCreate();
	plane->orientation = QMath::angleAxisRotation(90, Vec3(1.0f, 0.0f, 0.0f));
	planeShape->normal = QMath::rotate(planeShape->normal, plane->orientation);

	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();



	shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	cam = new Camera();
	cam->SkySetup("textures/hallpx.png",
		"textures/hallpy.png",
		"textures/hallpz.png",
		"textures/hallnx.png",
		"textures/hallny.png",
		"textures/hallnz.png"

	);
	cam->SetPosition(Vec3(0.0f, 2.0f, 5.5f));
	return true;
}

void Scene1p::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);
	sphere->OnDestroy();
	delete sphere;

	mesh->OnDestroy();
	delete mesh;

	plane->OnDestroy();
	delete plane;

	planeMesh->OnDestroy();
	delete planeMesh;

	shader->OnDestroy();
	delete shader;


}

void Scene1p::HandleEvents(const SDL_Event& sdlEvent) {
	cam->HandleEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_P:
			drawInWireMode = !drawInWireMode;
			break;
		case SDL_SCANCODE_W:
		{
			Quaternion rot = QMath::angleAxisRotation(-1, Vec3(1.0f, 0.0f, 0.0f));
			plane->orientation *= rot;
			planeShape->normal = QMath::rotate(planeShape->normal, rot);
			break;
		}
		case SDL_SCANCODE_S:
		{
			Quaternion rot = QMath::angleAxisRotation(1, Vec3(1.0f, 0.0f, 0.0f));
			plane->orientation *= rot;
			planeShape->normal = QMath::rotate(planeShape->normal, rot);
			break;
		}
		case SDL_SCANCODE_A:
		{
			Quaternion rot = QMath::angleAxisRotation(1, Vec3(0.0f, 0.0f, 1.0f));
			plane->orientation *= rot;
			planeShape->normal = QMath::rotate(planeShape->normal, rot);
			break;
		}
		case SDL_SCANCODE_D:
		{
			Quaternion rot = QMath::angleAxisRotation(-1, Vec3(0.0f, 0.0f, 1.0f));
			plane->orientation *= rot;
			planeShape->normal = QMath::rotate(planeShape->normal, rot);
			break;
		}
		}
		break;

	case SDL_MOUSEMOTION:
		break;

	case SDL_MOUSEBUTTONDOWN:
		break;

	case SDL_MOUSEBUTTONUP:
		break;

	default:
		break;
	}
}

void Scene1p::Update(const float deltaTime) {
	sphere->Update(deltaTime);
	float const gravity = -9.8f;
	float c = 0.2f;
	Vec3 gravityAcc(0.0f, gravity * sphere->mass, 0.0f);
	Vec3 force = (sphere->vel * -c) + gravityAcc;
	// Apply forces to the sphere
	sphere->ApplyForce(force);
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	float cosAngle = VMath::dot(up, VMath::normalize(planeShape->normal));
	float angle = acosf(cosAngle);
	float torqueMag = (sphere->mass * sphere->rad * sin(angle));
	Vec3 rotationAxis = VMath::cross(up, VMath::normalize(planeShape->normal));
	Vec3 torque = torqueMag * VMath::normalize(rotationAxis);
	sphere->ApplyTorque(torque);
	sphere->UpdateAngularVel(deltaTime);
	sphere->UpdateOrientation(deltaTime);



	float d = VMath::dot(planeShape->normal, sphere->pos);
	if (d < 1) {
		sphere->vel = VMath::cross(sphere->angularVel, VMath::normalize(planeShape->normal));
		sphere->pos += (d - 1) * -VMath::normalize(planeShape->normal);
	}

	sphere->UpdateVel(deltaTime);
	sphere->UpdatePos(deltaTime);


}

void Scene1p::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphere->GetModelMatrix());
	mesh->Render(GL_TRIANGLES);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, plane->GetModelMatrix());
	planeMesh->Render(GL_TRIANGLES);
	glUseProgram(0);
}