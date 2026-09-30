#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glew.h>
#include <MMath.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Engine/Body.h"
#include "Graphics/Camera.h"
#include "Graphics/Shader.h"

Camera::Camera() {
    projection = MMath::perspective(45.0f, (16.0f / 9.0f), 0.5f, 10000.0f);
    position = Vec3(0.0f, 0.0f, -25.0f);
    orientation = Quaternion(1.0f, Vec3(0.0f, 0.0f, 0.0f));
    pitchAngle = 0.0f;
    yaw = 0.0f;
    skybox = nullptr; // Fix: Initialize skybox to nullptr
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

void Camera::UpdateFrustum() {
    Matrix4 vp = GetProjectionMatrix() * GetViewMatrix();

    // Left
    frustumPlanes[0].normal = Vec3(
       vp[3] + vp[0],
       vp[7] + vp[4],
       vp[11] + vp[8]
    );
    frustumPlanes[0].d = vp[15] + vp[12];

    // Right
    frustumPlanes[1].normal = Vec3(
       vp[3] - vp[0],
       vp[7] - vp[4],
       vp[11] - vp[8]
    );
    frustumPlanes[1].d = vp[15] - vp[12];

    // Bottom
    frustumPlanes[2].normal = Vec3(
       vp[3] + vp[1],
       vp[7] + vp[5],
       vp[11] + vp[9]
    );
    frustumPlanes[2].d = vp[15] + vp[13];

    // Top
    frustumPlanes[3].normal = Vec3(
       vp[3] - vp[1],
       vp[7] - vp[5],
       vp[11] - vp[9]
    );
    frustumPlanes[3].d = vp[15] - vp[13];

    // Near
    frustumPlanes[4].normal = Vec3(
       vp[3] + vp[2],
       vp[7] + vp[6],
       vp[11] + vp[10]
    );
    frustumPlanes[4].d = vp[15] + vp[14];

    // Far
    frustumPlanes[5].normal = Vec3(
       vp[3] - vp[2],
       vp[7] - vp[6],
       vp[11] - vp[10]
    );
    frustumPlanes[5].d = vp[15] - vp[14];

    // Normalize planes
    for (int i = 0; i < 6; i++) {
       float len = VMath::mag(frustumPlanes[i].normal);
       frustumPlanes[i].normal = VMath::normalize(frustumPlanes[i].normal);
       frustumPlanes[i].d /= len;
    }
}

bool Camera::IsAABBVisible(const AABB& box) const
{
    for (int i = 0; i < 6; i++) {
       const Plane& p = frustumPlanes[i];

       // Pick the vertex most in the direction of the plane normal
       Vec3 positive(
          p.normal.x > 0 ? box.max.x : box.min.x,
          p.normal.y > 0 ? box.max.y : box.min.y,
          p.normal.z > 0 ? box.max.z : box.min.z
       );

       if (p.Distance(positive) < 0) {
          return false; // Completely outside of view
       }
    }
    return true;
}

void Camera::OnDestroy() {
    if (skybox) {
       skybox->OnDestroy();
       delete skybox;
       skybox = nullptr;
    }
}

Camera::~Camera() {}

bool Camera::OnCreate() { // uses default skybox setup
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

bool Camera::SkySetup(const char* posXFileName_, 
                      const char* posYFileName_, 
                      const char* posZFileName_, 
                      const char* negXFileName_, 
                      const char* negYFileName_, 
                      const char* negZFileName_) {
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
       if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
          event.button.button == SDL_BUTTON_LEFT) {
          mouseHeld = true;
          ignoreNextMouseDelta = true;
          
          // SDL3 window reference required for relative mouse mode
          SDL_Window* window = SDL_GetWindowFromID(event.button.windowID);
          if (window) {
              SDL_SetWindowRelativeMouseMode(window, true);
          }
       }
    }
    
    // Handle Mouse Release
    if (!m1Override) {
       if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
          event.button.button == SDL_BUTTON_LEFT) {
          mouseHeld = false;
          
          SDL_Window* window = SDL_GetWindowFromID(event.button.windowID);
          if (window) {
              SDL_SetWindowRelativeMouseMode(window, false);
          }
       }
    }
    
    // Handle Mouse Motion if mouse 1 is held down or if the mouse1 override is active
    if ((event.type == SDL_EVENT_MOUSE_MOTION && mouseHeld) || 
        (event.type == SDL_EVENT_MOUSE_MOTION && m1Override)) {
       if (ignoreNextMouseDelta) {
          ignoreNextMouseDelta = false;
          return;
       }
       // Get Mouse Deltas (event.motion.xrel / yrel are floats in SDL3)
       float dx = event.motion.xrel;
       float dy = event.motion.yrel;
       
       // Update yaw and pitch angles
       yaw -= dx * sensitivity;
       pitchAngle -= dy * sensitivity;
       
       // Clamp the pitch angle
       if (pitchAngle > 89.0f) pitchAngle = 89.0f;
       if (pitchAngle < -89.0f) pitchAngle = -89.0f;
       
       /// Create the orientation quaternion from yaw and pitch
       Quaternion yawQ = QMath::angleAxisRotation(yaw, Vec3(0.0f, 1.0f, 0.0f));
       Quaternion pitchQ = QMath::angleAxisRotation(pitchAngle, Vec3(1.0f, 0.0f, 0.0f));
       
       // Combine yaw and pitch and normalize the orientation
       orientation = yawQ * pitchQ;
       orientation = QMath::normalize(orientation);
    }
}

void Camera::Update(const float deltaTime) {
    if (!canCamMove) return;

    // SDL3 keyboard state return type is const bool*
    const bool* keys = SDL_GetKeyboardState(nullptr);
    float moveSpeed = 5.0f * deltaTime;

    // 1. Create a direction vector to accumulate movement
    Vec3 moveDir(0.0f, 0.0f, 0.0f);

    // 2. Accumulate all inputs into the direction vector
    if (keys[SDL_SCANCODE_W]) moveDir += GetCameraForward();
    if (keys[SDL_SCANCODE_S]) moveDir -= GetCameraForward();
    if (keys[SDL_SCANCODE_A]) moveDir -= GetCameraRight();
    if (keys[SDL_SCANCODE_D]) moveDir += GetCameraRight();

    if (keys[SDL_SCANCODE_E]) moveDir += Vec3(0.0f, 1.0f, 0.0f);
    if (keys[SDL_SCANCODE_Q]) moveDir -= Vec3(0.0f, 1.0f, 0.0f);

    // 3. Normalize and Apply
    if (VMath::mag(moveDir) > 0.001f) {
       moveDir = VMath::normalize(moveDir);
       SetPosition(GetPosition() + moveDir * moveSpeed);
    }
}

void Camera::RenderSkyBox() const {
    if (skybox == nullptr) return;
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glUseProgram(skybox->GetShader()->GetProgram());
    glUniformMatrix4fv(skybox->GetShader()->GetUniformID("projectionMatrix"), 1, GL_FALSE, projection);
    glUniformMatrix4fv(skybox->GetShader()->GetUniformID("viewMatrix"), 1, GL_FALSE,
       MMath::toMatrix4(QMath::conjugate(orientation)));     
    skybox->Render();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glUseProgram(0);
}