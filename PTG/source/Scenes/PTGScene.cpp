#include "Scenes/PTGScene.h"
#include <iostream>
#include <glew.h>
#include <SDL.h>
#include <MMath.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Graphics/Shader.h"
#include "Engine/Body.h"
#include "Graphics/Texture.h"
#include <glm/glm.hpp>
#include <QMath.h>



PTGScene::PTGScene() :
	tessShader(nullptr),
	mesh(nullptr),
	reflectionShader(nullptr),
	drawInWireMode(false),
	texture(nullptr),
	cam(nullptr),
	terrainMesh(nullptr),
	terrainTexture(nullptr),
	heightMap(nullptr),
	normalMap(nullptr),
	diffuseMap(nullptr),
	terrain(nullptr)
{
	Debug::Info("Created PTGScene: ", __FILE__, __LINE__);
}

PTGScene::~PTGScene() {
	Debug::Info("Deleted PTGScene: ", __FILE__, __LINE__);
}

bool PTGScene::OnCreate() {
	Debug::Info("Loading assets PTGScene: ", __FILE__, __LINE__);
	
	// Sphere Definition

	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	
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
	terrainModelMatrix = MMath::translate(0.0f, -1.0f, 0.0f) * MMath::rotate(10, Vec3(1.0f,0.0f,0.0f)) * MMath::scale(40.0f,1.0f,40.0f);

	
	// Height Map
	heightMap = new Texture;
	heightMap->LoadImage("textures/terrainHeight.png");

	// Normal Map
	normalMap = new Texture;
	normalMap->LoadImage("textures/terrainNormal.png");

	// Diffuse Map
	diffuseMap = new Texture;
	diffuseMap->LoadImage("textures/terrainDiffuse.png");
	
	// Camera Initialization
	cam = new Camera();
	cam->SkySetup("textures/hallpx.png",
		"textures/hallpy.png",
		"textures/hallpz.png",
		"textures/hallnx.png",
		"textures/hallny.png",
		"textures/hallnz.png"
	);
	cam->position = Vec3(0.0f, 2.0f, 10.0f);
	cam->setCamMovement(true);

	// Shaders
	tessShader = new Shader("shaders/tessalationVert.glsl", "shaders/tessalationFrag.glsl", 
		"shaders/tessalationCtrl.glsl", "shaders/tessalationEval.glsl");
	if (tessShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	reflectionShader = new Shader("shaders/reflectionVert.glsl", "shaders/reflectionFrag.glsl");
	if (reflectionShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	
	// Model Matrix
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	return true;

	Litpos = Vec3(0.0f, 10.0f, 5.0f);
}

void PTGScene::OnDestroy() {
	Debug::Info("Deleting assets PTGScene: ", __FILE__, __LINE__);
	mesh->OnDestroy();
	delete mesh;

	cam->OnDestroy();
	delete cam;

	terrain->OnDestroy();
	delete terrain;

	terrainMesh->OnDestroy();
	delete terrainMesh;


	tessShader->OnDestroy();
	delete tessShader;

	reflectionShader->OnDestroy();
	delete reflectionShader;
}

void PTGScene::HandleEvents(const SDL_Event& sdlEvent) {
	cam->HandleEvents(sdlEvent);

	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {

		case SDL_SCANCODE_O:
		{
			drawInWireMode = !drawInWireMode;

			break;
		}
		}
		break;

	case SDL_MOUSEMOTION:
		//cam->HandleEvents(sdlEvent);
		break;

	case SDL_MOUSEBUTTONDOWN:
		

		break;

	case SDL_MOUSEBUTTONUP:
		break;

	default:
		break;
	}
}

void PTGScene::Update(const float deltaTime) {

}

void PTGScene::Render() const {
	
	/// Set the background color then clear the screen
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(0.0f, 10.0f, 10.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	cam->RenderSkyBox();

	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//glDepthMask(GL_TRUE);

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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



