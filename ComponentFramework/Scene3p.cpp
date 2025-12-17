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
	, reflectionShader{ nullptr }
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
	tentacleIndex = 0;
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
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	reflectionShader = new Shader("shaders/reflectionVert.glsl", "shaders/reflectionFrag.glsl");
	if (reflectionShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
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
	cam->HandleEvents(sdlEvent);
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
		case SDL_SCANCODE_W: {
			jellyfishHead->pos = jellyfishHead->pos + Vec3(0.0f, 0.0f, 0.25f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.z--;
			}
			break;
		}

		case SDL_SCANCODE_S: {
			jellyfishHead->pos = jellyfishHead->pos + Vec3(0.0f, 0.0f, -0.25f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.z++;
			}
			break;
		}
		case SDL_SCANCODE_A: {
			jellyfishHead->pos = jellyfishHead->pos + Vec3(-0.25f, 0.0f, 0.0f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.x--;
			}
			break;
		}
		case SDL_SCANCODE_D: {
			jellyfishHead->pos = jellyfishHead->pos + Vec3(0.25f, 0.0f, 0.0f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.x++;
			}
			break;
		}
		case SDL_SCANCODE_SPACE: {
			jellyfishHead->vel = Vec3(0.0f, 0.0f, -1.0f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.y++;
			}
			break;
		}
		case SDL_SCANCODE_N:
			if (drawNormals == false) {
				drawNormals = true;
			}
			else {
				drawNormals = false;
			}
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
		

	//applying forces to the tentacle spheres
		for (int i = 0; i < 100; i++) {
			float dragCoeff = 2.5f;
			// Start off with laminar flow, so drag force = -cv 
			
			Vec3 dragForce = -dragCoeff * tentacleSpheres[i]->vel;
			Vec3 gravityForce = tentacleSpheres[i]->mass * Vec3(0, -10, 0);

			// TODO – Rod constraint physics!
			if (VMath::mag(tentacleSpheres[i]->vel) > 1.0f) {
				// Switch to turbulent flow if the spheres are moving fast
				// That means drag force = -cv^2
				dragForce = -dragCoeff * tentacleSpheres[i]->vel *
					VMath::mag(tentacleSpheres[i]->vel);
			}
			tentacleSpheres[i]->ApplyForce(gravityForce + dragForce);
			// calculate a first approximation of velocity based on acceleration 
			tentacleSpheres[i]->UpdateVel(deltaTime);
			if (anchorIndex == 10) {
				anchorIndex = 0;
			}
			if (tentacleIndex == 0) {
			tentacleSpheres[i]->RodConstraint(deltaTime, anchors[anchorIndex]->pos,spacing * tentacleIndex + 1);
			}
			else {
				Vec3 restraint = tentacleSpheres[i - 1]->pos;
				tentacleSpheres[i]->RodConstraint(deltaTime, restraint, spacing);
			}
		tentacleIndex++;
		if (tentacleIndex == 10) {
			anchorIndex += 1;
			tentacleIndex = 0;
		}
		// update position using corrected velocities based on rod constraint
		tentacleSpheres[i]->UpdatePos(deltaTime);
		// Match physics and graphics
		// Increment the index counter for the next swing through this nested loop
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


	

	glUseProgram(reflectionShader->GetProgram());
	glUniformMatrix4fv(reflectionShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(reflectionShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniformMatrix4fv(reflectionShader->GetUniformID("modelMatrix"), 1, GL_FALSE, jellyfishHead->GetModelMatrix());
	mesh->Render(GL_TRIANGLES);
	
	for (Body* anchor : anchors) {
		glUniformMatrix4fv(reflectionShader->GetUniformID("modelMatrix"), 1, GL_FALSE, anchor->GetModelMatrix());
		mesh->Render(GL_TRIANGLES);
	}
	
	for (int i = 0; i < 100; i++) {
		glUniformMatrix4fv(reflectionShader->GetUniformID("modelMatrix"), 1, GL_FALSE, tentacleSpheres[i]->GetModelMatrix());
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
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniform4fv(drawNormalsShader->GetUniformID("color"), 1, color);
	glUseProgram(0);

}