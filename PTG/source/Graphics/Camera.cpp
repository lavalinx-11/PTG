#include <SDL.h>
#include <MMath.h>
#include <glew.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Engine/Body.h"
#include "Graphics/Camera.h"
#include "Graphics/Shader.h"
#include <SDL_image.h>

Camera::Camera() {
	projection = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 10000.0f);
	position = Vec3(0.0f, 0.0f, -25.0f);
	orientation = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f));
	pitchAngle = 0.0f;
	yaw = 0.0f;
}

void Camera::SetView(const Quaternion& orientation_, const Vec3& position_) {
	orientation = orientation_;
	position = position_;
}

Vec3 Camera::GetCameraForward() {
	Matrix4 camToWorld = MMath::inverse(GetViewMatrix());
	Vec4 forwardCamSpace(0.0f, 0.0f, -1.0f, 0.0f);
	Vec4 forwardWorld = camToWorld * forwardCamSpace;
	return VMath::normalize(Vec3(forwardWorld.x, forwardWorld.y, forwardWorld.z));
}

Vec3 Camera::GetCameraRight() {
	Matrix4 camToWorld = MMath::inverse(GetViewMatrix());
	Vec4 rightCamSpace(1.0f, 0.0f, 0.0f, 0.0f);
	Vec4 rightWorld = camToWorld * rightCamSpace;
	return VMath::normalize(Vec3(rightWorld.x, rightWorld.y, rightWorld.z));
}

void Camera::OnDestroy() {
	if (skybox) {
		skybox->OnDestroy();
		delete skybox;
	}
}

Camera::~Camera() {}

bool Camera::OnCreate() {//uses default skybox setup
	skybox = new SkyBox("textures/StarSkyboxPosx.png",
						"textures/StarSkyboxPosy.png",
						"textures/StarSkyboxPosz.png",
						"textures/StarSkyboxNegx.png",
						"textures/StarSkyboxNegY.png",
						"textures/StarSkyboxnegz.png");
	std::cout << "working " << std::endl;
	
	if (skybox->OnCreate() == false) {
		return false;
	}
	return true;

}
//has a dynamic skybox setup which allows me to input any skybox I want while setting up the scene going through the camera
bool Camera::SkySetup(const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char* negYFileName_, const char* negZFileName_) {
	
	skybox = new SkyBox(posXFileName_,
						posYFileName_,
						posZFileName_,
						negXFileName_,
						negYFileName_,
						negZFileName_);
	if (skybox->OnCreate() == false) {
		return false;
	}
	return true;
}


void Camera::HandleEvents(const SDL_Event& event) {

	// Handle Mouse Events for Camera Rotation. Checks if the left mouse button is held down
	if (!m1Override) {
		if (event.type == SDL_MOUSEBUTTONDOWN &&
			event.button.button == SDL_BUTTON_LEFT) {
			mouseHeld = true;
			ignoreNextMouseDelta = true;
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
	}
	// Handle Mouse Release
	if (!m1Override) {
		if (event.type == SDL_MOUSEBUTTONUP &&
			event.button.button == SDL_BUTTON_LEFT) {
			mouseHeld = false;
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
	}
	// Handle Mouse Motion if mouse 1 is held down or if the mouse1 override is active
	if (event.type == SDL_MOUSEMOTION && mouseHeld || event.type == SDL_MOUSEMOTION && m1Override) {
		if (ignoreNextMouseDelta) {
			ignoreNextMouseDelta = false;
			return;
		}
		// Get Mouse Deltas
		float dx = (float)event.motion.xrel;
		float dy = (float)event.motion.yrel;
		// Rotation Sensitivity
		// Update yaw and pitch angles
		yaw -= dx * sensitivity;
		pitchAngle -= dy * sensitivity;
		// Clamp the pitch angle to avoid looking too far up or down 
		if (pitchAngle > 89.0f) pitchAngle = 89.0f;
		if (pitchAngle < -89.0f) pitchAngle = -89.0f;
		/// Create the orientation quaternion from yaw and pitch
		Quaternion yawQ = QMath::angleAxisRotation(yaw, Vec3(0.0f, 1.0f, 0.0f));
		Quaternion pitchQ = QMath::angleAxisRotation(pitchAngle, Vec3(1.0f, 0.0f, 0.0f));
		// Combine yaw and pitch and normalize the orientation
		orientation = yawQ * pitchQ;
		orientation = QMath::normalize(orientation);
	}


	float moveSpeed = 0.5f;
	if (canCamMove) {
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.scancode) {

			case SDL_SCANCODE_W: {

				SetPosition(GetPosition() + GetCameraForward() * moveSpeed);
				break;
			}

			case SDL_SCANCODE_S: {
				SetPosition(GetPosition() - GetCameraForward() * moveSpeed);
				break;
			}
			case SDL_SCANCODE_A: {
				SetPosition(GetPosition() - GetCameraRight() * moveSpeed);
				break;
			}
			case SDL_SCANCODE_D: {
				SetPosition(GetPosition() + GetCameraRight() * moveSpeed);
				break;
			}
			case SDL_SCANCODE_Q: {
				SetPosition(GetPosition() + Vec3(0.0f, moveSpeed, 0.0f));
				break;
			}
			case SDL_SCANCODE_E: {
				SetPosition(GetPosition() - Vec3(0.0f, moveSpeed, 0.0f));
				break;
			}
			}
		}
	}

}

void Camera::RenderSkyBox() const{
	if (skybox == nullptr) return;
	//depth test makes it so the things farther away are aren't drawn or shown
	glDisable(GL_CULL_FACE);//turns off depth test so that things that are within it are visible
	//cull face is a way of performance gain in opengl. "If you're seeing a texture/triangle that's going clockwise then it's backwards so it doesn't draw it to save performance
	glDisable(GL_DEPTH_TEST);
	glUseProgram(skybox->GetShader()->GetProgram());//goes through the skybox class to get the program id and turn on the shader
	glUniformMatrix4fv(skybox->GetShader()->GetUniformID("projectionMatrix"), 1, GL_FALSE, projection);//gets the projection matrix
	glUniformMatrix4fv(skybox->GetShader()->GetUniformID("viewMatrix"), 1, GL_FALSE,
		MMath::toMatrix4(QMath::conjugate(orientation)));		/// they are, then draw the cube.  
	skybox->Render();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glUseProgram(0);
}