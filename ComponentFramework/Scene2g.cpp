#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Scene2g.h"
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include "Trackball.h"
#include "Camera.h"
#include <QMath.h>


Scene2g::Scene2g() :
	skull(nullptr),
	leftEye(nullptr),
	rightEye(nullptr),
	leftEyeTexture(nullptr),
	rightEyeTexture(nullptr),
	leftEyeMesh(nullptr),
	rightEyeMesh(nullptr),
	shader(nullptr),
	mesh(nullptr),
	drawInWireMode(false),
	texture(nullptr),
	cam(nullptr)
{
	Debug::Info("Created Scene2: ", __FILE__, __LINE__);
}

Scene2g::~Scene2g() {
	Debug::Info("Deleted Scene2: ", __FILE__, __LINE__);
}

bool Scene2g::OnCreate() {
	Debug::Info("Loading assets Scene2: ", __FILE__, __LINE__);
	skull = new Body();
	skull->OnCreate();

	
	mesh = new Mesh("meshes/Skull.obj");
	mesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/skull_texture.jpg");

	leftEye = new Body();
	leftEye->OnCreate();
	leftEye->pos = Vec3(0.55f, 0.25f, 0.75f);
	leftEye->orientation = QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));
	leftEye->rad = 0.4f;

	leftEyeMesh = new Mesh("meshes/Sphere.obj");
	leftEyeMesh->OnCreate();

	leftEyeTexture = new Texture();
	leftEyeTexture->LoadImage("textures/evilEye.jpg");

	rightEye = new Body();
	rightEye->OnCreate();
	rightEye->pos = Vec3(-0.55f, 0.25f, 0.75f);
	rightEye->orientation = QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));
	rightEye->rad = 0.4f;

	rightEyeMesh = new Mesh("meshes/Sphere.obj");
	rightEyeMesh->OnCreate();

	rightEyeTexture = new Texture();
	rightEyeTexture->LoadImage("textures/evilEye.jpg");
	
	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	Litpos[0] = Vec3(0.0f, 5.0f, 0.0f);   // Light above the object
	Litpos[1] = Vec3(0.0f, -5.0f, 0.0f);  // Light below the object
	Litpos[2] = Vec3(5.0f, 0.0f, 5.0f);   // Light in front-right
	Litpos[3] = Vec3(-5.0f, 0.0f, 5.0f);  // Light in front-left
	Litpos[4] = Vec3(0.0f, 0.0f, -5.0f);  // Light behind the object


	Diffuse[0] = Vec4(0.0, 0.5, 0.5, 1.0);  // Strong purple (mix of red and blue)
	Diffuse[1] = Vec4(0.0, 1.0, 0.5, 1.0);  // Extra blue contribution
	Diffuse[2] = Vec4(0.0, 0.0, 0.5, 0.2);  // Soft purple mix
	Diffuse[3] = Vec4(0.2, 0.0, 0.0, 0.5);  // No contribution
	Diffuse[4] = Vec4(0.0, 0.5, 0.0, 0.5);  // No contribution


	Specular[0] = Vec4(0.5, 0.0, 0.5, 0.5);  // Strong white highlights
	Specular[1] = Vec4(0.0, 0.0, 1.0, 0.3);  // Blue highlights
	Specular[2] = Vec4(0.5, 0.0, 1.0, 0.3);  // Soft white-blue mix
	Specular[3] = Vec4(0.0, 0.0, 0.0, 0.3);  // No contribution
	Specular[4] = Vec4(0.0, 0.0, 0.0, 0.3);  // No contribution

	cam = new Camera();
	cam->OnCreate();
	cam->SetPosition(Vec3(0.0f, 0.0f, 8.0f));
	cam->setM1Override(true);
	cam->setCamSensitivity(0.02f);
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	return true;
}

void Scene2g::OnDestroy() {
	Debug::Info("Deleting assets Scene2: ", __FILE__, __LINE__);
	skull->OnDestroy();
	delete skull;

	mesh->OnDestroy();
	delete mesh;

	shader->OnDestroy();
	delete shader;

	cam->OnDestroy();
	delete cam;

	leftEye->OnDestroy();
	delete leftEye;

	rightEye->OnDestroy();
	delete rightEye;

	rightEyeMesh->OnDestroy();
	delete rightEyeMesh;

	leftEyeMesh->OnDestroy();
	delete leftEyeMesh;

}

void Scene2g::HandleEvents(const SDL_Event& sdlEvent) {
	trackball.HandleEvents(sdlEvent);
	cam->HandleEvents(sdlEvent);
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

void Scene2g::Update(const float deltaTime) {
	leftEye->orientation = QMath::inverse(skull->orientation) * QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));
	rightEye->orientation = QMath::inverse(skull->orientation) * QMath::angleAxisRotation(-90, Vec3(0.0f, 1.0f, 0.0f));

	skull->orientation = trackball.getQuat();
	skull->Update(deltaTime);
}

void Scene2g::Render() const {
	
	/// Set the background color then clear the screen
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	cam->RenderSkyBox();
	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, texture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, skull->GetModelMatrix());
	glUniform3fv(shader->GetUniformID("Litpos[0]"), 5, *Litpos);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, (GLfloat*)Specular);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"), 5, (GLfloat*)Diffuse);


	mesh->Render(GL_TRIANGLES);





	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, leftEyeTexture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, skull->GetModelMatrix() * leftEye->GetModelMatrix());
	glUniform3fv(shader->GetUniformID("Litpos[0]"), 5, *Litpos);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, (GLfloat*)Specular);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"), 5, (GLfloat*)Diffuse);


	leftEyeMesh->Render(GL_TRIANGLES);




	glUseProgram(shader->GetProgram());
	glBindTexture(GL_TEXTURE_2D, rightEyeTexture->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, skull->GetModelMatrix() * rightEye->GetModelMatrix());
	glUniform3fv(shader->GetUniformID("Litpos[0]"), 5, *Litpos);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, (GLfloat*)Specular);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"), 5, (GLfloat*)Diffuse);


	rightEyeMesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}



