#pragma once
/// The camera still doesn't exist mathamatically but there is utility in placeing the
/// view and projections matricies in class. Might as well call it a camera.
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>
#include <MMath.h>
#include <VMath.h>
#include <QMath.h>
#include "SkyBox.h"
using namespace MATH;

union SDL_Event;

class Camera {
private:
	Quaternion orientation;
	Matrix4 projection;
	Matrix4 view;
	SkyBox* skybox;
	Vec3 target = Vec3(0.0f, 0.0f, 0.0f);
	float yaw = 0.0f;
	float pitch = 0.0f;
	float pitchAngle = 0.0f;
	float sensitivity = 0.1f;
	bool mouseHeld = false;
	bool ignoreNextMouseDelta = false;
	bool canCamMove = false;
	bool m1Override = false;

public:
	Vec3 position;
	Camera();
	~Camera();
	bool OnCreate();
	bool SkySetup(const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char*
		negYFileName_, const char* negZFileName_);
	void OnDestroy();
	void RenderSkyBox() const;
	void HandleEvents(const SDL_Event& sdlEvent);

	Matrix4 GetViewMatrix() const {
		Quaternion viewQuat = QMath::conjugate(orientation);
		Matrix4 viewRotation = MMath::toMatrix4(viewQuat);
		Matrix4 viewTranslation = MMath::translate(-position.x, -position.y, -position.z);
		return viewRotation * viewTranslation;
	}

	Matrix4 GetProjectionMatrix() const { return projection; }

	Quaternion GetOrientation() const { return orientation; }

	void SetView(const Quaternion& orientation_, const Vec3& position_);

	Vec3 GetPosition() const { return position; }

	void SetPosition(Vec3 newPos) { position = newPos; }

	void setOrientaion(Quaternion newOri) { orientation = newOri; }

	int GetSkyTexID() { return skybox->GetTexture(); }
	Matrix4 GetViewMatrix2() const {

		const auto center_point = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 translated_position = center_point - target;

		return MMath::translate(position) *
			MMath::toMatrix4(orientation) *
			MMath::translate(translated_position);

	}
	void setCamMovement(bool canMove) { canCamMove = canMove; }
	void setM1Override(bool override) { m1Override = override; }
	void setTarget(Vec3 target_) { target = target_; }
	void setCamSensitivity(float sensitivity_) { sensitivity = sensitivity_; }
	// Getters for camera's worldspace directions
	Vec3 GetCameraForward();
	Vec3 GetCameraRight();
};
