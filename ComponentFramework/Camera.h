#pragma once
/// The camera still doesn't exist mathamatically but there is utility in placeing the
/// view and projections matricies in class. Might as well call it a camera.
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>
#include "SkyBox.h"
#include "Trackball.h"
using namespace MATH;

union SDL_Event;

class Camera {
private:
	Quaternion orientation;
	Matrix4 projection;
	Matrix4 view;
	Trackball trackball;
	SkyBox* skybox;
	Vec3 target = Vec3(0.0f, 0.0f, 0.0f);
public:
	Vec3 position;
	Camera();
	~Camera();
	bool OnCreate();
	bool SkySetup(const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char*
		negYFileName_, const char* negZFileName_);
	void OnDestroy();
	void RenderSkyBox() const;
	void HandelEvents(const SDL_Event& sdlEvent);

	/// Some getters and setters
	Matrix4 GetViewMatrix() const {

		//return MMath::inverse(MMath::toMatrix4(orientation)) * MMath::inverse(MMath::translate(position));
		return  MMath::translate(position) * MMath::toMatrix4(orientation);
	}

	Matrix4 GetProjectionMatrix() const {
		return projection;
	}

	void dontTrackY();


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

	void setTarget(Vec3 target_) { target = target_; }
};
