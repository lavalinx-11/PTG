#include "Scenes/Scene2p.h"
#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Graphics/Shader.h"
#include "Engine/Body.h"
#include "Engine/Plane.h"
#include "Graphics/SkyBox.h"
#include "Engine/Collision.h"
#include "Graphics/Texture.h"



Scene2p::Scene2p()
	: sphereA{ nullptr }
	, sphereB{ nullptr }
	, shader{ nullptr }
	, sphereAmesh{ nullptr }
	, sphereATex{ nullptr }
	, sphereBmesh{ nullptr }
	, sphereBTex{ nullptr }
	, planeTex{ nullptr }
	, drawInWireMode{ false }
	, plane{ nullptr }
	, planeMesh{ nullptr }
	, drawNormals{ true }
	, planeAngleRadians{ 0 }
	, cam{ nullptr }
	, skyblox { nullptr }
{
	Debug::Info("Created Scene2p: ", __FILE__, __LINE__);
}

Scene2p::~Scene2p() {
	Debug::Info("Deleted Scene2p: ", __FILE__, __LINE__);
}

bool Scene2p::OnCreate() {
	Debug::Info("Loading assets Scene2p: ", __FILE__, __LINE__);
	sphereA = new Body();
	sphereA->OnCreate();
	sphereA->rad = 1;
	sphereA->pos.y = 0.0f;

	sphereAmesh = new Mesh("meshes/Sphere.obj");
	sphereAmesh->OnCreate();

	sphereATex = new Texture();
	sphereATex->LoadImage("textures/evilEye.jpg");
	

	sphereB = new Body();
	sphereB->OnCreate();
	sphereB->rad = 1;
	sphereB->pos = Vec3(0.5f, 0.0f, -5.0f);


	sphereB->pos.y = sphereA->pos.y;
	sphereBmesh = new Mesh("meshes/Sphere.obj");
	sphereBmesh->OnCreate();

	sphereBTex = new Texture();
	sphereBTex->LoadImage("textures/evilEye.jpg");

	plane = new Body();
	plane->OnCreate();
	plane->pos = Vec3(0.0f, -1.25f, 0.0f);
	// Initially, the plane normal points to your nose
	planeNormal.set(0, 0, 1);

	plane->orientation = QMath::angleAxisRotation(85, Vec3(-1, 0, 0));

	// Rotate normal based on the plane's orientation
	planeNormal = QMath::rotate(planeNormal, plane->orientation);


	planeMesh = new Mesh("meshes/Plane.obj");
	planeMesh->OnCreate();

	planeTex = new Texture();
	planeTex->LoadImage("textures/subTexture.jpg");


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
	cam->position = Vec3(0.0f, 2.0f, 8.0f);
	cam->setCamMovement(true);
	shader = new Shader("shaders/texturePhongVert.glsl", "shaders/texturePhongFrag.glsl");
	//shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");

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

	return true;
}

void Scene2p::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);
	sphereA->OnDestroy();
	delete sphereA;

	sphereB->OnDestroy();
	delete sphereB;

	plane->OnDestroy();
	delete plane;

	sphereAmesh->OnDestroy();
	delete sphereAmesh;

	sphereBmesh->OnDestroy();
	delete sphereBmesh;

	planeMesh->OnDestroy();
	delete planeMesh;

	shader->OnDestroy();
	delete shader;

	cam->OnDestroy();
	delete cam;

}

void Scene2p::HandleEvents(const SDL_Event& sdlEvent) {
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
		case SDL_SCANCODE_N:
			if (drawNormals == false) {
				drawNormals = true;
			}
			else {
				drawNormals = false;
			}
			break;

		case SDL_SCANCODE_SPACE:
			// Whack the cue ball along the direction we are looking
			// Scott says we always look down the -z. Umer believes!!!
			// Make velocity a direction (w = 0)
			Vec4 changeInVelCameraSpace = Vec4(0, 0, -5, 0);
			Matrix4 worldToCamera = cam->GetViewMatrix();
			Matrix4 cameraToWorld = MMath::inverse(worldToCamera);
			Vec4 changeInVelWorldSpace = cameraToWorld * changeInVelCameraSpace;

			//sphereA->vel += Vec3(changeInVelWorldSpace.x, 0.0f, changeInVelWorldSpace.z);
			sphereA->vel.x += changeInVelWorldSpace.x;
			sphereA->vel.z += changeInVelWorldSpace.z;

			// Following Umer's scribbles on the board to 
			// convert velocity into an angular velocity 
			Vec3 normalizedVel = VMath::normalize(sphereA->vel);
			// Normalized rVector is just the planeNormal
			Vec3 axisOfRotation = VMath::cross(planeNormal, normalizedVel);
			float angularVelMagnitude = VMath::mag(sphereA->vel) / sphereA->rad;
			//sphereA->angularVel = angularVelMagnitude * axisOfRotation;
			sphereA->angularVel.x = angularVelMagnitude * axisOfRotation.x;
			sphereA->angularVel.z = angularVelMagnitude * axisOfRotation.z;


			// Then another cross product to recalcuate the linear velocity
			// to keep the ball on the plane
			Vec3 rVector = planeNormal * sphereA->rad;
			sphereA->vel = VMath::cross(sphereA->angularVel, rVector);
			std::cout << "WHACK!\n";
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



void Scene2p::Update(const float deltaTime) {
	// Detect collision between spheres
	drawNormals = false;
		if (COLLISION::SphereSphereCollisionDetected(sphereA, sphereB)) {
			std::cout << "COLLISION!\n";
			COLLISION::SphereSphereCollisionResponse(sphereA, sphereB);

			Vec3 normalizedVel = VMath::normalize(sphereA->vel);
			Vec3 axisOfRotation = VMath::cross(planeNormal, normalizedVel);
			float angularVelMagnitude = VMath::mag(sphereA->vel) / sphereA->rad;
			sphereA->angularVel.x = angularVelMagnitude * axisOfRotation.x;
			sphereA->angularVel.z = angularVelMagnitude * axisOfRotation.z;			
	}
		sphereA->angularAcc = sphereA->accel;
	sphereA->UpdateOrientation(deltaTime);
	sphereA->UpdatePos(deltaTime);
	sphereB->UpdatePos(deltaTime);

	if (sphereB->pos.y > 0 || sphereA->pos.y > 0) {
		sphereB->pos.y = 0;
		sphereA->pos.y = 0;
	}

	if (sphereA->pos.x <= -5.0f || sphereA->pos.x >= 5.0f) {//checks if the ball has hit any of the borders and flips gravity with decay on velocity
		if (sphereA->pos.x > 5.0f) {
			sphereA->pos.x = 5.0f;
		}
		if (sphereA->pos.x < -5.0f) {
			sphereA->pos.x = -5.0f;
		}
		sphereA->vel.x *= -1.0f;
	}
	if (sphereA->pos.z <= -5.0f || sphereA->pos.z >= 5.0f) {
		if (sphereA->pos.z > 5.0f) {
			sphereA->pos.z = 5.0f;
		}
		if (sphereA->pos.z < -5.0f) {
			sphereA->pos.z = -5.0f;
		}
		sphereA->vel.z *= -1.0f;
	}

}

void Scene2p::Render() const {
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
	glBindTexture(GL_TEXTURE_2D, sphereATex->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereA->GetModelMatrix());
	glUniform3fv(shader->GetUniformID("Litpos[0]"), 5, *Litpos);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, (GLfloat*)Specular);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"), 5, (GLfloat*)Diffuse);
	sphereAmesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, sphereBTex->getTextureID());
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereB->GetModelMatrix());
	glUniform3fv(shader->GetUniformID("Litpos[0]"), 5, *Litpos);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, (GLfloat*)Specular);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"), 5, (GLfloat*)Diffuse);
	sphereBmesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, planeTex->getTextureID()); 
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, plane->GetModelMatrix());
	glUniform3fv(shader->GetUniformID("Litpos[0]"), 5, *Litpos);
	glUniform4fv(shader->GetUniformID("Specular[0]"), 5, (GLfloat*)Specular);
	glUniform4fv(shader->GetUniformID("Diffuse[0]"), 5, (GLfloat*)Diffuse);
	planeMesh->Render(GL_TRIANGLES);
	glUseProgram(0);

	/// Added by Scott
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	if (drawNormals == true) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.5f));
	}
}


void Scene2p::DrawNormals(const Vec4 color) const {

	glUseProgram(drawNormalsShader->GetProgram());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniform4fv(drawNormalsShader->GetUniformID("color"), 1, color);

	glUniformMatrix4fv(drawNormalsShader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereA->GetModelMatrix());
	sphereAmesh->Render(GL_TRIANGLES);

	glUniformMatrix4fv(drawNormalsShader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereB->GetModelMatrix());
	sphereBmesh->Render(GL_TRIANGLES);

	glUniformMatrix4fv(drawNormalsShader->GetUniformID("modelMatrix"), 1, GL_FALSE, plane->GetModelMatrix());
	planeMesh->Render(GL_TRIANGLES);

	glUseProgram(0);

}