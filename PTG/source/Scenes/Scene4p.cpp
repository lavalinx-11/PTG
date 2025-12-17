#include "Scenes/Scene4p.h"
#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include <Plane.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Engine/Body.h"
#include "Engine/Collision.h"
#include "Graphics/Shader.h"
#include "Graphics/SkyBox.h"
#include "Graphics/Texture.h"
#include "TMath.h"
#include "Dot.h"	
using namespace MATHEX;

Scene4p::Scene4p()
	: triangleBody{ nullptr }
	, colourShader{ nullptr }
	, shader{ nullptr }
	, triangleMesh{ nullptr }
	, sphereA{ nullptr }
	, sphereAmesh{ nullptr }
	, sphereATex{ nullptr }
	, drawInWireMode{ true }
	, drawNormals{ true }
	, planeAngleRadians{ 0 }
	, cam{ nullptr }
	, skyblox { nullptr }
	, pointOnPlane{ nullptr }
	, pointOnLine01{ nullptr }
	, pointOnLine12{ nullptr }
	, pointOnLine20{ nullptr }
	, collisionPoint{ nullptr }
{
	Debug::Info("Created Scene4p: ", __FILE__, __LINE__);
}

Scene4p::~Scene4p() {
	Debug::Info("Deleted Scene4p: ", __FILE__, __LINE__);
}

bool Scene4p::OnCreate() {
	Debug::Info("Loading assets Scene4p: ", __FILE__, __LINE__);
	
	//Sphere
	sphereA = new Body();
	sphereA->OnCreate();
	sphereA->pos = Vec3(0.0f, 0.0f, 3.0f);

	sphereAmesh = new Mesh("meshes/Sphere.obj");
	sphereAmesh->OnCreate();



	//Triangle
	triangleBody = new Body();
	triangleBody->OnCreate();
	triangleBody->pos = Vec3(0.0f, 0.0f, 0.0f);


	triangleShape = new Triangle(
		Vec3(-2.0f, -1.0f, 0.0f),
		Vec3(2.0f, -1.0f, 0.0f),
		Vec3(0.0f, 3.0f, 0.0f)
	);

	triangleMesh = new Mesh();
	triangleMesh->OnCreate(triangleShape);


	//Point on plane
	pointOnPlane = new Body();
	pointOnPlane->OnCreate();
	pointOnPlane->rad = 0.25;


	//Points
	pointOnLine01 = new Body();
	pointOnLine01->OnCreate();
	pointOnLine01->rad = 0.25;

	pointOnLine12 = new Body();
	pointOnLine12->OnCreate();
	pointOnLine12->rad = 0.25;

	pointOnLine20 = new Body();
	pointOnLine20->OnCreate();
	pointOnLine20->rad = 0.25;


	//Collision Point
	collisionPoint = new Body();
	collisionPoint->OnCreate();
	collisionPoint->rad = 0.3;




	//Shaders
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
	cam->position = Vec3(0.0f, 2.0f, 10.0f);
	shader = new Shader("shaders/defaultVert.glsl", "shaders/defaultFrag.glsl");
	if (shader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}

	colourShader = new Shader("shaders/defaultVert.glsl", "shaders/colourFrag.glsl");
	if (colourShader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}
	
	return true;
}

void Scene4p::OnDestroy() {
	Debug::Info("Deleting assets Scene1: ", __FILE__, __LINE__);

	//Sphere
	sphereA->OnDestroy();
	delete sphereA;

	sphereAmesh->OnDestroy();
	delete sphereAmesh;

	//Triangle
	triangleMesh->OnDestroy();
	delete triangleMesh;

	triangleBody->OnDestroy();
	delete triangleBody;

	//Points
	pointOnLine01->OnDestroy();
	delete pointOnLine01;

	pointOnLine12->OnDestroy();
	delete pointOnLine12;

	pointOnLine20->OnDestroy();
	delete pointOnLine20;


	//Shader
	shader->OnDestroy();
	delete shader;

	colourShader->OnDestroy();
	delete colourShader;

	drawNormalsShader->OnDestroy();
	delete drawNormalsShader;

	//Camera
	cam->OnDestroy();
	delete cam;

	//Point on plane
	pointOnPlane->OnDestroy();
	delete pointOnPlane;


	//Collision Point
	collisionPoint->OnDestroy();
	delete collisionPoint;

}

void Scene4p::HandleEvents(const SDL_Event& sdlEvent) {
	cam->HandleEvents(sdlEvent);
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		switch (sdlEvent.key.keysym.scancode) {
		case SDL_SCANCODE_P:
			drawInWireMode = !drawInWireMode;
			break;
		
		case SDL_SCANCODE_W: {
			sphereA->pos = sphereA->pos + Vec3(0.0f, 0.25f, 0.0f);
			break;
		}

		case SDL_SCANCODE_S: {
			sphereA->pos = sphereA->pos + Vec3(0.0f, -0.25f, 0.0f);
			break;
		}
		case SDL_SCANCODE_A: {
			sphereA->pos = sphereA->pos + Vec3(-0.25f, 0.0f, 0.0f);
			break;
		}
		case SDL_SCANCODE_D: {
			sphereA->pos = sphereA->pos + Vec3(0.25f, 0.0f, 0.0f);
			break;
		}
		case SDL_SCANCODE_SPACE: {
			sphereA->vel = Vec3(0.0f, 0.0f, -1.0f);
			break;
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
		}
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



void Scene4p::Update(const float deltaTime) {
	sphereA->UpdatePos(deltaTime);

	//Projecting the center of the sphere onto the triangle
	Vec4 pointOnPlane4d = project(sphereA->pos, TMath::getPlane(*triangleShape));
	pointOnPlane->pos = VMath::perspectiveDivide(pointOnPlane4d);

	//Projecting the sphere onto the lines of the triangle
	DualQuat line01 = triangleShape->getV0() & triangleShape->getV1();
	DualQuat line12 = triangleShape->getV1() & triangleShape->getV2();
	DualQuat line20 = triangleShape->getV2() & triangleShape->getV0();

	//why not working?
	Vec4 pointOn01 = project(sphereA->pos, line01);
	pointOnLine01->pos = VMath::perspectiveDivide(pointOn01);
	Vec4 pointOn12 = project(sphereA->pos, line12);
	pointOnLine12->pos = VMath::perspectiveDivide(pointOn12);
	Vec4 pointOn20 = project(sphereA->pos, line20);
	pointOnLine20->pos = VMath::perspectiveDivide(pointOn20);

	//collision point
	if (TMath::isPointInsideTriangle(pointOnPlane->pos, *triangleShape)) {
		collisionPoint->pos = pointOnPlane->pos;
	}
	else {
		// What are the two closest edges to the point on the plane?
		float dist01 = fabs(DQMath::orientedDist(pointOnPlane->pos, line01));
		float dist12 = fabs(DQMath::orientedDist(pointOnPlane->pos, line12));
		float dist20 = fabs(DQMath::orientedDist(pointOnPlane->pos, line20));
		if (dist01 <= dist12 && dist01 <= dist20) {
			// If line01 is the closest line, then use that projected position
			collisionPoint->pos = pointOnLine01->pos;
			// Hold on, what if that position is outide the triangle?
			if (!TMath::isPointInsideTriangle(collisionPoint->pos, *triangleShape)) {
				if (dist12 <= dist20) {
					collisionPoint->pos = pointOnLine12->pos;
				}
				else {
					collisionPoint->pos = pointOnLine20->pos;
				}
			}
		}
		else if (dist12 <= dist01 && dist12 <= dist20) {
			// In this case, line12 is the closest line
			collisionPoint->pos = pointOnLine12->pos;
			// But wait! What if that is not inside the triangle?
			if (!TMath::isPointInsideTriangle(collisionPoint->pos, *triangleShape)) {
				if (dist01 <= dist20) {
					collisionPoint->pos = pointOnLine01->pos;
				}
				else {
					collisionPoint->pos = pointOnLine20->pos;
				}
			}
		}
		else {
			// Final case is where line20 is the closest line
			collisionPoint->pos = pointOnLine20->pos;
			// Hold your horses! What if that is not inside the triangle?
			if (!TMath::isPointInsideTriangle(collisionPoint->pos, *triangleShape)) {
				if (dist01 <= dist12) {
					collisionPoint->pos = pointOnLine01->pos;
				}
				else {
					collisionPoint->pos = pointOnLine12->pos;
				}
			}
		}
	}


		// Now that you have the collision point, let's check if we will collide
		// I'm tired after all this coding. Let's use Scott's distance method
		if (VMath::distance(sphereA->pos, collisionPoint->pos) <= sphereA->rad) {
			// Find the collision normal vector
			Vec3 collisionNormal = VMath::normalize(sphereA->pos - collisionPoint->pos);
			// Reflect the sphere off the triangle
			// Scott to the rescue again with his reflect method
			sphereA->vel = VMath::reflect(sphereA->vel, collisionNormal);
		}

	drawNormals = false;
	}



void Scene4p::Render() const {
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Shader setup
	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(shader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());

	//Triangle
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, triangleBody->GetModelMatrix());
	triangleMesh->Render(GL_TRIANGLES);

	//Sphere
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, sphereA->GetModelMatrix());
	sphereAmesh->Render(GL_TRIANGLES);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Point on Plane
	glUseProgram(colourShader->GetProgram());

	glUniformMatrix4fv(colourShader->GetUniformID("modelMatrix"), 1, GL_FALSE,pointOnPlane->GetModelMatrix());
	glUniform4fv(static_cast<GLint>(colourShader->GetUniformID("color_for_frag")), 1, Vec4(1.0f, 0.0f, 0.0f, 0.0f));
	sphereAmesh->Render(GL_TRIANGLES);
	


	//Point on lines
	glUniformMatrix4fv(colourShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(colourShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());

	glUniformMatrix4fv(colourShader->GetUniformID("modelMatrix"), 1, GL_FALSE, pointOnLine01->GetModelMatrix());
	glUniform4fv(static_cast<GLint>(colourShader->GetUniformID("color_for_frag")), 1, Vec4(0.0f, 1.0f, 1.0f, 0.0f));
	sphereAmesh->Render(GL_TRIANGLES);
	
	glUniformMatrix4fv(colourShader->GetUniformID("modelMatrix"), 1, GL_FALSE, pointOnLine12->GetModelMatrix());
	glUniform4fv(static_cast<GLint>(colourShader->GetUniformID("color_for_frag")), 1, Vec4(1.0f, 1.0f, 0.0f, 0.0f));
	sphereAmesh->Render(GL_TRIANGLES);
	
	glUniformMatrix4fv(colourShader->GetUniformID("modelMatrix"), 1, GL_FALSE, pointOnLine20->GetModelMatrix());
	glUniform4fv(static_cast<GLint>(colourShader->GetUniformID("color_for_frag")), 1, Vec4(1.0f, 0.0f, 1.0f, 0.0f));
	sphereAmesh->Render(GL_TRIANGLES);


	//Collision Point
	glUniformMatrix4fv(colourShader->GetUniformID("modelMatrix"), 1, GL_FALSE, collisionPoint->GetModelMatrix());
	glUniform4fv(static_cast<GLint>(colourShader->GetUniformID("color_for_frag")), 1, Vec4(1.0f, 1.0f, 1.0f, 0.0f));
	sphereAmesh->Render(GL_TRIANGLES);
	
	/// Added by Scott
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	if (drawNormals == true) {
		DrawNormals(Vec4(1.0f, 1.0f, 0.0f, 0.5f));
	}
}


void Scene4p::DrawNormals(const Vec4 color) const {

	glUseProgram(drawNormalsShader->GetProgram());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("projectionMatrix"), 1, GL_FALSE, cam->GetProjectionMatrix());
	glUniformMatrix4fv(drawNormalsShader->GetUniformID("viewMatrix"), 1, GL_FALSE, cam->GetViewMatrix());
	glUniform4fv(drawNormalsShader->GetUniformID("color"), 1, color);
	glUseProgram(0);

}