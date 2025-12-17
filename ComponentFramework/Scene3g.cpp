#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Scene3g.h"
#include "Debug.h"
#include "Mesh.h"
#include "Shader.h"
#include "Body.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include "Camera.h"
#include <QMath.h>


Scene3g::Scene3g() :
	sub(nullptr),
	reflectionShader(nullptr),
	mesh(nullptr),
	drawInWireMode(false),
	texture(nullptr),
	cam(nullptr)
{
	Debug::Info("Created Scene2: ", __FILE__, __LINE__);
}

Scene3g::~Scene3g() {
	Debug::Info("Deleted Scene2: ", __FILE__, __LINE__);
}

bool Scene3g::OnCreate() {
	Debug::Info("Loading assets Scene2: ", __FILE__, __LINE__);
	sub = new Body();
	sub->OnCreate();
	sub->pos = Vec3(0.0f, 0.0f, 0.0f);

	mesh = new Mesh("meshes/Sphere.obj");
	mesh->OnCreate();

	texture = new Texture();
	texture->LoadImage("textures/evilEye.jpg");

	
	reflectionShader = new Shader("shaders/reflectionVert.glsl", "shaders/reflectionFrag.glsl");
	if (reflectionShader->OnCreate() == false) {
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
	/*cam->SkySetup("textures/UnderwaterPosx.png",
				"textures/UnderwaterPosy.png",
				"textures/UnderwaterPosz.png",
				"textures/UnderwaterNegx.png",
				"textures/UnderwaterNegY.png",
				"textures/Underwaternegz.png"
	
	);*/
	cam->SkySetup("textures/hallpx.png",
		"textures/hallpy.png",
		"textures/hallpz.png",
		"textures/hallnx.png",
		"textures/hallny.png",
		"textures/hallnz.png"
	);
	cam->position = Vec3(0.0f, 0.0f, 7.0f);
	modelMatrix = MMath::toMatrix4(Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0)));
	return true;
}

void Scene3g::OnDestroy() {
	Debug::Info("Deleting assets Scene2: ", __FILE__, __LINE__);
	sub->OnDestroy();
	delete sub;

	mesh->OnDestroy();
	delete mesh;

	cam->OnDestroy();
	delete cam;
}

void Scene3g::HandleEvents(const SDL_Event& sdlEvent) {
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

void Scene3g::Update(const float deltaTime) {
	cam->setTarget(sub->pos);
}

void Scene3g::Render() const {
	
	/// Set the background color then clear the screen
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (drawInWireMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


	cam->RenderSkyBox();


	glUseProgram(reflectionShader->GetProgram());
	glBindTexture(GL_TEXTURE_CUBE_MAP, cam->GetSkyTexID()); 
	glUniformMatrix4fv(reflectionShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(reflectionShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	Vec3 forwardDirection = Vec3(0.0, 0.0, -1.0);
	Vec3 rotatedDirection = QMath::rotate(forwardDirection, cam->GetOrientation());
	Vec3 cameraEffectivePosition =
		Vec3(
			rotatedDirection.x,
			rotatedDirection.y,
			-rotatedDirection.z
		)
		* 25.0;
	cameraEffectivePosition.print();
	glUniform3fv(
		static_cast<GLint>(reflectionShader->GetUniformID("camPos")),
		1,
		cameraEffectivePosition
	);
	glUniformMatrix4fv(reflectionShader->GetUniformID("modelMatrix"), 1, GL_FALSE, sub->GetModelMatrix());

	mesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glUseProgram(0);
}



