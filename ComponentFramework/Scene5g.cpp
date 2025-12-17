#include "Scene5g.h"
#include <iostream>
#include <glew.h>
#include <SDL.h>
#include <MMath.h>
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <QMath.h>
#include "Texture.h"


Scene5g::Scene5g()
	: jellyfishHead{ nullptr }
	, tentacleSpheres{ nullptr }
	, reflectionShader{ nullptr }
	, shader{ nullptr }
	, tessShader{ nullptr }
	, texture { nullptr }
	, terrainMesh {nullptr }
	, terrainTexture { nullptr }
	, heightMap { nullptr }
	, normalMap { nullptr }
	, diffuseMap { nullptr }
	, terrain { nullptr }
	, drawInWireMode{ true }
	, drawNormals{ true }
	, mesh{ nullptr }
	, planeAngleRadians{ 0 }
	, cam{ nullptr }
	, skyblox { nullptr }
{
	Debug::Info("Created Scene5g: ", __FILE__, __LINE__);
}

Scene5g::~Scene5g() {
	Debug::Info("Deleted Scene5g: ", __FILE__, __LINE__);
}

bool Scene5g::OnCreate() {
	Debug::Info("Loading assets Scene5g: ", __FILE__, __LINE__);

	// Sphere Definition
	sub = new Body();
	sub->OnCreate();
	sub->pos = Vec3(0.0f, 1.0f, 0.0f);

	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/evilEye.jpg");

	// Tessalation Check
	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	// Terrain Initialization
	terrain = new Body();
	terrain->OnCreate();
	terrain->pos = Vec3(0.0f, 0.0f, 0.0f);
	terrain->orientation = QMath::angleAxisRotation(90, Vec3(-1, 0, 0));

	terrainMesh = new Mesh("meshes/Plane.obj");
	terrainMesh->OnCreate();
	terrainTexture = new Texture;
	terrainTexture->LoadImage("textures/checkboardText.png");
	terrainModelMatrix = MMath::translate(0.0f, -1.0f, 0.0f) * MMath::rotate(10, Vec3(1.0f, 0.0f, 0.0f)) * MMath::scale(40.0f, 1.0f, 40.0f);


	// Height Map
	heightMap = new Texture;
	heightMap->LoadImage("textures/terrainHeight.png");

	// Normal Map
	normalMap = new Texture;
	normalMap->LoadImage("textures/terrainNormal.png");

	// Diffuse Map
	diffuseMap = new Texture;
	diffuseMap->LoadImage("textures/terrainDiffuse.png");


	// Umer Jellyfish Stuff
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


	
	
	cam = new Camera();
	cam->SkySetup("textures/hallpx.png",	
		"textures/hallpy.png",
		"textures/hallpz.png",
		"textures/hallnx.png",
		"textures/hallny.png",
		"textures/hallnz.png"

	);
	cam->position = Vec3(0.0f, 1.0f, 3.5f);

	// Shaders
	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	drawNormalsShader = new Shader("shaders/normalVert.glsl", "shaders/normalFrag.glsl", nullptr, nullptr, "shaders/normalGeom.glsl");
	if (drawNormalsShader->OnCreate() == false) {
		std::cout << "drawNormalsShader failed ... we have a problem\n";
	}

	reflectionShader = new Shader("shaders/reflectionVert.glsl", "shaders/reflectionFrag.glsl");
	if (reflectionShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	tessShader = new Shader("shaders/tessalationVert.glsl", "shaders/tessalationFrag.glsl",
		"shaders/tessalationCtrl.glsl", "shaders/tessalationEval.glsl");
	if (tessShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	
	return true;
}

void Scene5g::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);
	sub->OnDestroy();
	delete sub;

	mesh->OnDestroy();
	delete mesh;

	terrain->OnDestroy();
	delete terrain;

	terrainMesh->OnDestroy();
	delete terrainMesh;

	tessShader->OnDestroy();
	delete tessShader;

	reflectionShader->OnDestroy();
	delete reflectionShader;

	jellyfishHead->OnDestroy();
	delete jellyfishHead;

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

void Scene5g::HandleEvents(const SDL_Event& sdlEvent) {
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
			jellyfishHead->pos = jellyfishHead->pos + Vec3(0.0f, 0.0f, 1.0f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.z++;
			}
			break;
		}

		case SDL_SCANCODE_S: {
			jellyfishHead->pos = jellyfishHead->pos + Vec3(0.0f, 0.0f, -1.0f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.z--;
			}
			break;
		}
		case SDL_SCANCODE_A: {
			jellyfishHead->pos = jellyfishHead->pos + Vec3(-1.0f, 0.0f, 0.0f);
			for (int i = 0; i < 10; i++) {
				anchors[i]->pos.x--;
			}
			break;
		}
		case SDL_SCANCODE_D: {
			jellyfishHead->pos = jellyfishHead->pos + Vec3(1.0f, 0.0f, 0.0f);
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
		case SDL_SCANCODE_T:
		{
			if (tessLevel < 31) {
				tessLevel = tessLevel + 1;
			}
			break;
		}
		case SDL_SCANCODE_Y:
		{
			if (tessLevel > 0) {
				tessLevel = tessLevel - 1;
			}
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



void Scene5g::Update(const float deltaTime) {
	jellyfishHead->UpdatePos(deltaTime);





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



	// This code went in my nested loop. Looping over all the anchors and then looping 
	// over all the spheres per anchor

	// Umer will just do this for one tentacle (you need to do all of them)
	
	


void Scene5g::Render() const {
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


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap->getTextureID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap->getTextureID());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, diffuseMap->getTextureID());

	glUseProgram(tessShader->GetProgram());
	glUniformMatrix4fv(tessShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(tessShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniformMatrix4fv(tessShader->GetUniformID("modelMatrix"), 1, GL_FALSE, terrain->GetModelMatrix());
	glUniform1f(tessShader->GetUniformID("tessalationLev"), tessLevel);
	glUniform3fv(tessShader->GetUniformID("lightPos"), 1, Litpos);
	terrainMesh->Render(GL_PATCHES);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

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


void Scene5g::DrawNormals(const Vec4 color) const {

	glUseProgram(drawNormalsShader->GetProgram());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniform4fv(drawNormalsShader->GetUniformID("color"), 1, color);
	glUseProgram(0);

}