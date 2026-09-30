#include <glew.h>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <MMath.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Engine/Body.h"
#include "Graphics/SkyBox.h"
#include "Graphics/Shader.h"

SkyBox::SkyBox(const char* posXFileName_, 
               const char* posYFileName_, 
               const char* posZFileName_, 
               const char* negXFileName_, 
               const char* negYFileName_, 
               const char* negZFileName_) {
    posXFileName = posXFileName_;
    posYFileName = posYFileName_;
    posZFileName = posZFileName_;
    negXFileName = negXFileName_;
    negYFileName = negYFileName_;
    negZFileName = negZFileName_;
}

SkyBox::~SkyBox() {

}

Shader* SkyBox::GetShader() {
    return Skyshader;
}

bool SkyBox::LoadImages() {
    glGenTextures(1, &textureID); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); 

    const char* fileNames[6] = {
        posXFileName, posYFileName, posZFileName,
        negXFileName, negYFileName, negZFileName
    };

    GLenum targets[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for (int i = 0; i < 6; ++i) {
        SDL_Surface* textureSurface = IMG_Load(fileNames[i]);
        if (textureSurface == nullptr) {
            std::cout << "Failed to load skybox texture: " << fileNames[i] << " - " << SDL_GetError() << "\n";
            return false;
        }

        // SDL3: Retrieve pixel format details to check bytes per pixel
        const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(textureSurface->format);
        if (details == nullptr) {
            SDL_DestroySurface(textureSurface);
            return false;
        }

        GLenum mode = (details->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(
            targets[i], 
            0, 
            mode, 
            textureSurface->w, 
            textureSurface->h, 
            0, 
            mode, 
            GL_UNSIGNED_BYTE, 
            textureSurface->pixels
        );

        // SDL3: SDL_FreeSurface is replaced by SDL_DestroySurface
        SDL_DestroySurface(textureSurface);
    }

    /// Wrapping and filtering options
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

bool SkyBox::OnCreate() {
    // Create shader
    Skyshader = new Shader("shaders/skyboxVert.glsl", "shaders/skyboxFrag.glsl");
    if (Skyshader->OnCreate() == false) {
        std::cout << "Shader failed ... we have a problem\n";
        return false;
    }

    // Create a mesh
    skyMesh = new Mesh("meshes/Cube.obj");
    if (skyMesh->OnCreate() == false) {
        return false;
    }

    // Call load image
    return LoadImages();
}

void SkyBox::OnDestroy() {

}

void SkyBox::Render() {
    glDepthMask(GL_FALSE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); // binding the cubemap texture
    skyMesh->Render(GL_TRIANGLES);                 // renders the mesh in triangle mode
    glDepthMask(GL_TRUE);
}