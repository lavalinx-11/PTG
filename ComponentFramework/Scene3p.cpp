#include "Scene3p.h"
#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "SkyBox.h"
#include "Texture.h"



Scene3p::Scene3p()
	: jellyfishHead{ nullptr }
	, tentacleSpheres{ nullptr }
	, shader{ nullptr }
	, drawInWireMode{ true }
	, drawNormals{ true }
	, mesh{ nullptr }
	, planeAngleRadians{ 0 }
	, cam{ nullptr }
	, skyblox { nullptr }
{
	Debug::Info("Created Scene3p: ", __FILE__, __LINE__);
}

Scene3p::~Scene3p() {
	Debug::Info("Deleted Scene3p: ", __FILE__, __LINE__);
}

bool Scene3p::OnCreate() {
	Debug::Info("Loading assets Scene3p: ", __FILE__, __LINE__);

	

	jellyfishHead = new Body();
	jellyfishHead->OnCreate();
	jellyfishHead->rad = 6;
	jellyfishHead->pos.set(-1.5, 4, -25);

	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	const int numAnchors = 10;
	Vec3 anchorPos(-6.0f, 0.0f, -25);
	for (int i = 0; i < numAnchors; i++) {
		anchors.push_back(new Body());
		anchors[i]->pos = anchorPos;
		anchors[i]->rad = 0.5f;
		// Move the anchor position for the next swing through this loop 
		anchorPos += Vec3(spacing, 0, 0);
	}
	tentacleSpheres[0] = new Body();
	for (int i = 0; i < numSpheresPerAnchor; i++) {
		tentacleSpheres.push_back(new Body());
		tentacleSpheres[i]->rad = 0.1f;
		tentacleSpheres[i]->pos = anchors[anchorIndex]->pos - Vec3(0, spacing * tentacleIndex + 1, 0);
		tentacleSpheres[i]->mass = 1;
		tentacleIndex++;
		if (tentacleIndex == 10) {
			anchorIndex += 1;
			tentacleIndex = 0;
		}
	}
	anchorIndex = 0;

	drawNormalsShader = new Shader("shaders/normalVert.glsl", "shaders/normalFrag.glsl", nullptr, nullptr, "shaders/normalGeom.glsl");
	if (drawNormalsShader->OnCreate() == false) {
		std::cout << "drawNormalsShader failed ... we have a problem\n";
	}

	
	
	cam = new Camera();
	cam->SkySetup("textures/hallpx.png",
		"textures/hallpy.png",
		"textures/hallpz.png",
		"textures/hallnx.png",
		"textures/hallny.png",
		"textures/hallnz.png"

	);
	cam->position = Vec3(0.0f, -2.0f, -5.5f);
	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	//shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");

	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}


	cam->dontTrackY();
	return true;
}

void Scene3p::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);

	jellyfishHead->OnDestroy();
	delete jellyfishHead;

	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;

	for (auto anchor : anchors) {
		anchor->OnDestroy();
		delete anchor;
	}

	for (auto tentacleSphere : tentacleSpheres) {
		tentacleSphere->OnDestroy();
		delete tentacleSphere;
	}

	cam->OnDestroy();
	delete cam;

}

void Scene3p::HandleEvents(const SDL_Event& sdlEvent) {
	trackball.HandleEvents(sdlEvent);
	cam->HandelEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_P:
			drawInWireMode = !drawInWireMode;
			break;
		
		case SDL_SCANCODE_M:
		{
			Vec3 posi = Vec3(0.0f, 0.0f, 0.0f);

			posi += Vec3(0.0f, 1.0f, 0.0f);
			cam->SetPosition(posi);
		}
		break;
		case SDL_SCANCODE_N:
			if (drawNormals == false) {
				drawNormals = true;
			}
			else {
				drawNormals = false;
			}
			break;

		case SDL_SCANCODE_SPACE:
			
			break;
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



void Scene3p::Update(const float deltaTime) {
	jellyfishHead->UpdatePos(deltaTime);

	Vec3 offset = Vec3(0.0f, -2.0f, 40.0f);
	Vec3 rotatedOffset = QMath::rotate(offset, cam->GetOrientation());
	cameraPos = (jellyfishHead->pos + rotatedOffset);
	cam->SetView(cam->GetOrientation(), cameraPos);
	// This code went in my nested loop. Looping over all the anchors and then looping 
	// over all the spheres per anchor

	// Umer will just do this for one tentacle (you need to do all of them)
	for (int i = 0; i < 10; i++) {
		float dragCoeff = 0.5f;
		// Start off with laminar flow, so drag force = -cv 
		Vec3 dragForce = -dragCoeff * tentacleSpheres[i]->vel;
	

		Vec3 gravityForce = tentacleSpheres[i]->mass * Vec3(0, -10, 0);
		Vec3 windForce = Vec3(0, 0, 0);
		tentacleSpheres[i]->ApplyForce(gravityForce + dragForce + windForce);
		// calculate a first approximation of velocity based on acceleration 
		tentacleSpheres[i]->UpdateVel(deltaTime);

		// Straight line constraint
		float slope = 5;
		float y_intercept = 0;
		//tentacleSpheres[i]->StraightLineConstraint(slope, y_intercept, deltaTime);

		// Quadratic constraint
		float a = 0.1;
		float b = 0;
		float c = -5;
		//tentacleSpheres[i]->QuadraticConstraint(a, b, c, deltaTime);

		float r = 1;
		//tentacleSpheres[i]->CircleConstraint(r, circleCentrePos, deltaTime);
		{
			Vec3 circleCentrePos = tentacleSpheres[7]->pos;
			tentacleSpheres[8]->CircleConstraint(r, circleCentrePos, deltaTime);
		}
		{
			Vec3 circleCentrePos = tentacleSpheres[8]->pos;
			tentacleSpheres[9]->CircleConstraint(r, circleCentrePos, deltaTime);
		}




















		// update position using corrected velocities based on rod constraint 
		tentacleSpheres[8]->UpdatePos(deltaTime);
		tentacleSpheres[9]->UpdatePos(deltaTime);
	}

	
}

void Scene3p::Render() const {
	/// Set the background color then clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	cam->RenderSkyBox();

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}



	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewAlt());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, jellyfishHead->GetModelMatrix());
	mesh->Render(GL_TRIANGLES);
	
	for (Body* anchor : anchors) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, anchor->GetModelMatrix());
		mesh->Render(GL_TRIANGLES);
	}

	for (Body* tentacleSphere : tentacleSpheres) {
		glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, tentacleSphere->GetModelMatrix());
		mesh->Render(GL_TRIANGLES);
	}
	

	/// Added by Scott
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	if (drawNormals == true) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.5f));
	}
}


void Scene3p::DrawNormals(const Vec4 color) const {

	glUseProgram(drawNormalsShader->GetProgram());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewAlt());
	glUniform4fv(drawNormalsShader->GetUniformID("color"), 1, color);
	glUseProgram(0);

}