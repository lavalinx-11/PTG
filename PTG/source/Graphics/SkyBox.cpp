#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <MMath.h>
#include "Engine/Debug.h"
#include "Engine/Mesh.h"
#include "Engine/Body.h"
#include "Graphics/SkyBox.h"
#include "Graphics/Shader.h"
#include <SDL_image.h>

SkyBox::SkyBox(const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char* negYFileName_, const char* negZFileName_) {
	posXFileName = posXFileName_;
	posYFileName = posYFileName_;
	posZFileName = posZFileName_;
	negXFileName = negXFileName_;
	negYFileName = negYFileName_;
	negZFileName = negZFileName_;

}

SkyBox::~SkyBox() {

}

Shader* SkyBox::GetShader()
{
	return Skyshader;

}

bool SkyBox::LoadImages() {
	glGenTextures(1, &textureID); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); 

	{
		SDL_Surface* textureSurface = IMG_Load(posXFileName);
		if (textureSurface == nullptr) {
			return true;
		}
		int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
		SDL_FreeSurface(textureSurface);
	}

	{
		SDL_Surface* textureSurface = IMG_Load(posYFileName);
		if (textureSurface == nullptr) {
			return true;
		}
		int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
		SDL_FreeSurface(textureSurface);
	}

	{
		SDL_Surface* textureSurface = IMG_Load(posZFileName);
		if (textureSurface == nullptr) {
			return true;
		}
		int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
		SDL_FreeSurface(textureSurface);
	}


	{
		SDL_Surface* textureSurface = IMG_Load(negXFileName);
		if (textureSurface == nullptr) {
			return true;
		}
		int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
		SDL_FreeSurface(textureSurface);
	}


	{
		SDL_Surface* textureSurface = IMG_Load(negYFileName);
		if (textureSurface == nullptr) {
			return true;
		}
		int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
		SDL_FreeSurface(textureSurface);
	}

	{
		SDL_Surface* textureSurface = IMG_Load(negZFileName);
		if (textureSurface == nullptr) {
			return true;
		}
		int mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
		SDL_FreeSurface(textureSurface);
	}



	/// Wrapping and filtering options
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

bool SkyBox::OnCreate() {
	//create shader

	Skyshader = new Shader("shaders/skyboxVert.glsl", "shaders/skyboxFrag.glsl");
	if (Skyshader->OnCreate() == false) {
		std::cout << "Shader failed ... we have a problem\n";
	}


	//create a mesh
	skyMesh = new Mesh("meshes/Cube.obj");
	skyMesh->OnCreate();

	//call load image
	LoadImages();
	return true;
}

void SkyBox::OnDestroy() {

}

void SkyBox::Render() {
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);//binding the cubemap texture
	skyMesh->Render(GL_TRIANGLES);//renders the mesh in triangle mode
	glDepthMask(GL_TRUE);
}

