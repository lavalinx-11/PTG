#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Scenes/Scene1g.h"
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Engine/Body.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Trackball.h"
#include <glm/glm.hpp>


Scene1g::Scene1g() : mario(nullptr), shader(nullptr), mesh(nullptr),
drawInWireMode(false), texture(nullptr) {
	Debug::Info("Created Scene0: ", __FILE__, __LINE__);
}

Scene1g::~Scene1g() {
	Debug::Info("Deleted Scene0: ", __FILE__, __LINE__);
}

bool Scene1g::OnCreate() {
	Debug::Info("Loading assets Scene0: ", __FILE__, __LINE__);
	mario = new Body();
	mario->OnCreate();


	mesh = new Mesh("meshes/Mario.obj");
	mesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/mario_main.png");

	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	Litpos[0] = Vec3(1.0f, 0.0f, 0.0f);
	Litpos[1] = Vec3(0.0f, 1.0f, 0.0f);
	Litpos[2] = Vec3(0.0f, 0.0f, 1.0f);
	Litpos[3] = Vec3(2.0f, 0.0f, 0.0f);
	Litpos[4] = Vec3(-2.0f, 2.0f, 0.0f);

	Diffuse[0] = Vec4(0.3, 0.2, 0.1, 0.0);
	Diffuse[1] = Vec4(0.6, 0.4, 0.2, 0.0);
	Diffuse[2] = Vec4(0.8, 0.6, 0.3, 0.0);
	Diffuse[3] = Vec4(0.5, 0.1, 0.4, 0.0);
	Diffuse[4] = Vec4(0.6, 0.6, 0.3, 0.0);


	Specular[0] = Vec4(0.4, 0.3, 0.5, 0.0);
	Specular[1] = Vec4(0.2, 0.6, 0.4, 0.0);
	Specular[2] = Vec4(0.4, 0.3, 0.3, 0.0);
	Specular[3] = Vec4(0.5, 0.3, 0.3, 0.0);
	Specular[4] = Vec4(0.6, 0.3, 0.3, 0.0);




	projectionMatrix = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 100.0f);
	viewMatrix = MMath::lookAt(Vec3(0.0f, 0.0f, 5.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	return true;
}

void Scene1g::OnDestroy() {
	Debug::Info("Deleting assets Scene0: ", __FILE__, __LINE__);
	mario->OnDestroy();
	delete mario;

	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;


}

void Scene1g::HandleEvents(const SDL_Event& sdlEvent) {
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_W:
			drawInWireMode = !drawInWireMode;
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

void Scene1g::Update(const float deltaTime) {
	modelMatrix *= MMath::rotate(2, 0.0f, 1.0f, 0.0f);
	mario->Update(deltaTime);
}

void Scene1g::Render() const {
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
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
	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrix);
	//glUniform3fv(shader->GetUniformID("lightPos"), 1, lightPos);
	glUniform3fv(shader->GetUniformID("Litpos[0]"), 5, *Litpos);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, (GLfloat*)Specular);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"), 5, (GLfloat*)Diffuse);


	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_2D, 0);
}



