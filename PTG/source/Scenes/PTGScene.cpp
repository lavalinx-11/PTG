#include "Scenes/PTGScene.h"
#include <iostream>
#include <glew.h>
#include <SDL2/SDL.h>
#include <MMath.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Graphics/Shader.h"
#include "Engine/Body.h"
#include "Graphics/Texture.h"
#include <glm/glm.hpp>
#include <QMath.h>


PTGScene::PTGScene() :
	mesh(nullptr),
	drawInWireMode(false),
	cam(nullptr),
	terrainShader(nullptr),
	chunkManager(nullptr)
{
	Debug::Info("Created PTGScene: ", __FILE__, __LINE__);
}

PTGScene::~PTGScene() {
	Debug::Info("Deleted PTGScene: ", __FILE__, __LINE__);
}

bool PTGScene::OnCreate() {
	Debug::Info("Loading assets PTGScene: ", __FILE__, __LINE__);
	

	terrainShader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (terrainShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	chunkManager = new ChunkManager(
		50,     // chunk resolution
		40.0f,  // chunk size
		1337,   // seed
		10      // view radius
	);
	

	//chunkManager->ShowChunksInViewToggle();
	// Camera Initialization
	cam = new Camera();
	/*cam->SkySetup("textures/hallpx.png",
		"textures/hallpy.png",
		"textures/hallpz.png",
		"textures/hallnx.png",
		"textures/hallny.png",
		"textures/hallnz.png"
	);*/
	cam->SetPosition(Vec3(0.0f, 2.0f, 10.0f));
	cam->setCamMovement(true);

	
	// Model Matrix
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	return true;
}




void PTGScene::OnDestroy() {
	Debug::Info("Deleting assets PTGScene: ", __FILE__, __LINE__);

	cam->OnDestroy();
	delete cam;

	delete chunkManager;
	chunkManager = nullptr;
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
	chunkManager->Update(*cam);
	cam->Update(deltaTime);	
}

void PTGScene::Render() const {
	// --- Update camera frustum for this frame ---
	cam->UpdateFrustum();

	// --- OpenGL state ---
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// --- Skybox ---
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	cam->RenderSkyBox();

	// --- Wireframe toggle ---
	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// --- Bind shader ---
	glUseProgram(terrainShader->GetProgram());

	// --- Cache uniform locations  ---
	GLuint uProjection = terrainShader->GetUniformID("projectionMatrix");
	GLuint uView = terrainShader->GetUniformID("viewMatrix");
	GLuint uModel = terrainShader->GetUniformID("modelMatrix");
	GLuint uDebugColor = terrainShader->GetUniformID("debugColor");

	// --- Upload camera matrices ---
	glUniformMatrix4fv(
		uProjection,
		1,
		GL_FALSE,
		cam->GetProjectionMatrix()
	);

	glUniformMatrix4fv(
		uView,
		1,
		GL_FALSE,
		cam->GetViewMatrix()
	);

	Matrix4 model(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glUniformMatrix4fv(
		uModel,
		1,
		GL_FALSE,
		model
	);

	chunkManager->Render(*cam, *terrainShader);


	


	// --- Cleanup ---
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	
	


	
}



