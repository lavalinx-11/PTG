#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Scenes/SceneManager.h"
#include "Engine/Debug.h"

int main(int argc, char* args[]) {
	static_assert(sizeof(void*) == 8, "This program requires a 64-bit build environment.");

	Debug::DebugInit("GameEngineLog.txt");
    
	SceneManager* gsm = new SceneManager();
	if (gsm->Initialize("Game Engine", 1920, 1080) == true) {
		gsm->Run();
	} 
	delete gsm;
	_CrtDumpMemoryLeaks();
	return 0;
}