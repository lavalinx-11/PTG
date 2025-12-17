#include "Engine/Body.h"
#include <QMath.h>
#include <MMath.h>
Body::Body() : pos{}, vel{}, accel{}, mass{ 1.0f }, mesh{ nullptr }, texture{ nullptr } {
}

Body::~Body() {}

void Body::Update(float deltaTime) {
    /// From 1st semester physics class
    pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
    vel += accel * deltaTime;
}

void Body::ApplyForce(Vec3 force) {
    accel = force / mass;
}

void Body::UpdateOrientation(float deltaTime) {
    //dir of angvel is rotaation axis
    //magn is angle
    float angleRad = VMath::mag(angularVel);
    float angleDeg = angleRad * RADIANS_TO_DEGREES;
    //if angle is zero kill
    if (angleRad < VERY_SMALL) { // no use == 0 for floats 
        return;
    }
    Vec3 axis = VMath::normalize(angularVel);
    Quaternion rotation = QMath::angleAxisRotation((angleDeg * deltaTime), axis);
    orientation = rotation * orientation;

}

void Body::ApplyTorque(Vec3 torque) {
    float test = (2.0f / 5.0f) * mass * (rad * rad);
    if (test < VERY_SMALL) {
        std::cout << "bad" << std::endl;
        return;
    }
    angularAcc = MMath::inverse(rotationalInertia) * torque;

}

void Body::UpdatePos(float deltaTime) {
    pos += vel * deltaTime + 0.5f * accel * deltaTime * deltaTime;
}

void Body::UpdateVel(float deltaTime) {
    vel += accel * deltaTime;
}

const Matrix4 Body::GetModelMatrix() const
{
    Matrix4 T = MMath::translate(pos);
    Matrix4 R = MMath::toMatrix4(orientation);
    Matrix4 S = MMath::scale(Vec3(rad, rad, rad));
    return(T * R * S);
}

void Body::UpdateAngularVel(float deltaTime) {
    angularVel += angularAcc * deltaTime;
}

bool Body::OnCreate() {
    //make rotational inertia in apply torque to make it able to be manipulate
    //
    rotationalInertia.loadIdentity();
    const float sphereI = (2.0f / 5.0f) * mass * (rad * rad);
    rotationalInertia = MMath::scale(sphereI, sphereI, sphereI);
    return true;
}

void Body::StraightLineConstraint(float slope, float y_intercept, float deltaTime)
{
	float positionConstraint = pos.y - slope * pos.x - y_intercept;
	float JV = vel.y - slope * vel.x; // this is the velocity constraint

	float baumgarteStabilizationParameter = 0.1;
	float bias = baumgarteStabilizationParameter * positionConstraint / deltaTime;
	float JJT = slope * slope + 1;
	float lagrangianMultiplier = (-JV - bias) / ((1 / mass) * JJT); // this is lambda
	Vec3 JT = Vec3(-slope, 1, 0); // Jacobian transposed
	Vec3 changeInVel = (1 / mass) * JT * lagrangianMultiplier;
	vel += changeInVel;
}

void Body::QuadraticConstraint(float a, float b, float c, float deltaTime)
{
	float positionConstraint = a * pos.x * pos.x + b * pos.x + c - pos.y;
	float JV = 2 * a * pos.x * vel.x + b * vel.x - vel.y;
	Vec3 JT = Vec3(2 * a * pos.x + b, -1, 0); // Jacobian transposed
	float baumgarteStabilizationParameter = 0.1;
	float bias = baumgarteStabilizationParameter * positionConstraint / deltaTime;
	float JJT = (2 * a * pos.x + b) * (2 * a * pos.x + b) + 1;
	float lagrangianMultiplier = (-JV - bias) / ((1 / mass) * JJT); // this is lambda
	Vec3 changeInVel = (1 / mass) * JT * lagrangianMultiplier;
	vel += changeInVel;

}

void Body::CircleConstraint(float r, Vec3 circleCentrePos, float deltaTime)
{
	float positionConstraint =
		(pos.x - circleCentrePos.x) * (pos.x - circleCentrePos.x)
		+ (pos.y - circleCentrePos.y) * (pos.y - circleCentrePos.y)
		- r * r;

	float JV =
		(2 * pos.x - 2 * circleCentrePos.x) * vel.x
		+ (2 * pos.y - 2 * circleCentrePos.y) * vel.y;

	Vec3 JT = Vec3(2 * pos.x - 2 * circleCentrePos.x, 2 * pos.y - 2 * circleCentrePos.y, 0);

	float baumgarteStabilizationParameter = 0.1;
	float bias = baumgarteStabilizationParameter * positionConstraint / deltaTime;

	float JJT = (2 * pos.x - 2 * circleCentrePos.x) * (2 * pos.x - 2 * circleCentrePos.x)
		+ (2 * pos.y - 2 * circleCentrePos.y) * (2 * pos.y - 2 * circleCentrePos.y);

	float lagrangianMultiplier = (-JV - bias) / ((1 / mass) * JJT); // this is lambda
	Vec3 changeInVel = (1 / mass) * JT * lagrangianMultiplier;
	vel += changeInVel;

}

void Body::SphereConstraint(float r, Vec3 sphereCentrePos, float deltaTime)
{
	// TODO for YOU
	// Look at the circleConstraint for clues on how to code this...
}

void Body::RodConstraint(float deltaTime, Vec3 anchorPoint, float rodLength) {
    Vec3 rodVector = anchorPoint - pos; // set it to the vector from body’s position to the anchor point
    float positionConstraint = VMath::mag(rodVector) - rodLength; // set to the magnitude of the rodVector minus the 
    if (VMath::mag(rodVector) < VERY_SMALL) { return; }
    float JV = VMath::dot(rodVector, vel) / MATH::VMath::mag(rodVector);
    const float baumgarteStabilizationParameter = 0.18; // Try tuning this number
    float b = -(baumgarteStabilizationParameter / deltaTime) * positionConstraint;
    float lambda = -mass * (JV + b);

    Vec3 Jtransposed = rodVector / VMath::mag(rodVector);
    Vec3 deltaVel = Jtransposed * lambda / mass;
    vel += deltaVel;
}



void Body::OnDestroy() {
}

void Body::Render() const {
}