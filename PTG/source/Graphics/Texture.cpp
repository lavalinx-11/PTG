#include "Graphics/Texture.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

Texture::Texture() {
	textureID = 0;
}

bool Texture::LoadImage(const char* filename) {
	SDL_Surface* textureSurface = IMG_Load(filename);
	if (textureSurface == nullptr) {
		return false;
	}

	// In SDL3, BytesPerPixel is inside the SDL_PixelFormatDetails structure,
	// accessed via SDL_GetPixelFormatDetails(surface->format)
	const SDL_PixelFormatDetails* details = SDL_GetPixelFormatDetails(textureSurface->format);
	if (details == nullptr) {
		SDL_DestroySurface(textureSurface);
		return false;
	}

	GLenum mode = (details->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE,textureSurface->pixels);
	SDL_DestroySurface(textureSurface);

	// Wrapping and filtering options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

	return true;
}

Texture::~Texture() {
	if (textureID != 0) {
		glDeleteTextures(1, &textureID);
	}
}