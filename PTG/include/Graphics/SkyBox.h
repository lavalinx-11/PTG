#pragma once
#include "Vector.h"
#include <Matrix.h>
#include <QMath.h>
#include "Plane.h"
using namespace MATH;

/// Forward declarations 
union SDL_Event;
class Body;
class Mesh;
class Shader;

class SkyBox {
private:
	Shader* Skyshader;
	Mesh* skyMesh;
	unsigned int textureID;
	const char* posXFileName;
	const char* posYFileName;
	const char* posZFileName;
	const char* negXFileName;
	const char* negYFileName;
	const char* negZFileName;


public:

	SkyBox(const char* posXFileName_, const char* posYFileName_, const char* posZFileName_, const char* negXFileName_, const char*
		negYFileName_, const char* negZFileName_);
		~SkyBox();
	Shader* GetShader();
		bool LoadImages();
		void Render();
		bool OnCreate();
		void OnDestroy();
		int GetTexture() const { return textureID; }

};

