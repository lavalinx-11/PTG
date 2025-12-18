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
	mesh(nullptr),
	drawInWireMode(false),
	cam(nullptr),
	terrainShader(nullptr)
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

	
	
	// Camera Initialization
	cam = new Camera();
	/*cam->SkySetup("textures/hallpx.png",
		"textures/hallpy.png",
		"textures/hallpz.png",
		"textures/hallnx.png",
		"textures/hallny.png",
		"textures/hallnz.png"
	);*/
	cam->position = Vec3(0.0f, 2.0f, 10.0f);
	cam->setCamMovement(true);

	
	// Model Matrix
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	return true;
}

inline int WorldToChunk(float worldPos, float chunkSize) {
	return static_cast<int>(std::floor(worldPos / chunkSize));
}


void PTGScene::OnDestroy() {
	Debug::Info("Deleting assets PTGScene: ", __FILE__, __LINE__);

	cam->OnDestroy();
	delete cam;


	for (auto& pair : chunks) {
		delete pair.second;
	}
	chunks.clear();
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
	int camChunkX = WorldToChunk(cam->position.x, chunkSize);
	int camChunkZ = WorldToChunk(cam->position.z, chunkSize);


	for (int z = camChunkZ - viewRadius; z <= camChunkZ + viewRadius; z++) {
		for (int x = camChunkX - viewRadius; x <= camChunkX + viewRadius; x++) {

			long long key = ChunkKey(x, z);

			if (chunks.find(key) == chunks.end()) {
				chunks[key] = new TerrainChunk(
					x, z,
					chunkResolution,
					chunkSize,
					seed
				);
			}
		}
	}

	for (auto it = chunks.begin(); it != chunks.end(); ) {
		TerrainChunk* chunk = it->second;

		int dx = std::abs(chunk->GetChunkX() - camChunkX);
		int dz = std::abs(chunk->GetChunkZ() - camChunkZ);

		if (dx > viewRadius || dz > viewRadius) {
			delete chunk;
			it = chunks.erase(it);
		}
		else {
			it++;
		}
	}
}

void PTGScene::Render() const {
	
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


	glUseProgram(terrainShader->GetProgram());

	glUniformMatrix4fv(
		terrainShader->GetUniformID("projectionMatrix"),
		1, GL_FALSE, cam->GetProjectionMatrix()
	);

	glUniformMatrix4fv(
		terrainShader->GetUniformID("viewMatrix"),
		1, GL_FALSE, cam->GetViewMatrix()
	);

	Matrix4 model = Matrix4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glUniformMatrix4fv(
		terrainShader->GetUniformID("modelMatrix"),
		1,
		GL_FALSE,
		model
	);

	for (const auto& pair : chunks) {
		pair.second->Render(*terrainShader);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



