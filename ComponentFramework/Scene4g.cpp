#include "Scene4g.h"
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


Scene4g::Scene4g() :
	sub(nullptr),
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
	Debug::Info("Created Scene4g: ", __FILE__, __LINE__);
}

Scene4g::~Scene4g() {
	Debug::Info("Deleted Scene4g: ", __FILE__, __LINE__);
}

bool Scene4g::OnCreate() {
	Debug::Info("Loading assets Scene4g: ", __FILE__, __LINE__);
	
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
	cam->position = Vec3(0.0f, 1.0f, 3.0f);
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

void Scene4g::OnDestroy() {
	Debug::Info("Deleting assets Scene4g: ", __FILE__, __LINE__);
	sub->OnDestroy();
	delete sub;

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

void Scene4g::HandleEvents(const SDL_Event& sdlEvent) {
	cam->HandleEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_P:
				{
			drawInWireMode = !drawInWireMode;

			break;
			}

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

void Scene4g::Update(const float deltaTime) {

}

void Scene4g::Render() const {
	
	/// Set the background color then clear the screen
	glEnable(GL_DEPTH_TEST);
	
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



