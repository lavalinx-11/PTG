#include "Graphics/Window.h"
#include "Engine/Debug.h"
#include <SDL3/SDL.h>

Window::Window(): window{nullptr}, context{nullptr}, width{0}, height{0} {}

Window::~Window() {
    OnDestroy();
}

bool Window::OnCreate(std::string name_, int width_, int height_) {
    // SDL3 returns false on failure instead of negative integer codes
    if (!SDL_Init(SDL_INIT_VIDEO)) {
       Debug::FatalError("Failed to initialize SDL: " + std::string(SDL_GetError()), __FILE__, __LINE__);
       return false;
    }
    
    this->width = width_;
    this->height = height_;

    // In SDL3, window placement options like SDL_WINDOWPOS_CENTERED are omitted when creating a window;
    // use SDL_CreateWindow (which defaults to centered/system placement) or SDL_CreateWindowWithPosition.
    window = SDL_CreateWindow(name_.c_str(), width, height, SDL_WINDOW_OPENGL);

    if (window == nullptr) {
       Debug::FatalError("Failed to create a window: " + std::string(SDL_GetError()), __FILE__, __LINE__);
       return false;
    }

    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
       Debug::FatalError("Failed to create OpenGL context: " + std::string(SDL_GetError()), __FILE__, __LINE__);
       return false;
    }

    int major, minor;
    getInstalledOpenGLInfo(&major, &minor);
    setAttributes(major, minor);

    /// Fire up the GL Extension Wrangler (GLEW)
    GLenum err = glewInit();
    if (err != GLEW_OK) {
       Debug::FatalError("Glew initialization failed", __FILE__, __LINE__);
       return false;
    }
    glViewport(0, 0, width, height);
    return true;
}

void Window::OnDestroy() {
    if (context) {
        SDL_GL_DestroyContext(context);
        context = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}

void Window::getInstalledOpenGLInfo(int *major, int *minor) {
    /// You can get some info regarding versions and manufacturer
    const GLubyte *version = glGetString(GL_VERSION);
    /// You can also get the version as ints   
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    glGetIntegerv(GL_MAJOR_VERSION, major);
    glGetIntegerv(GL_MINOR_VERSION, minor);
    Debug::Info("OpenGL version: " + std::string((char*)version), __FILE__, __LINE__);
    Debug::Info("Graphics card vendor: " + std::string((char*)vendor), __FILE__, __LINE__);
    Debug::Info("Graphics card name: " + std::string((char*)renderer), __FILE__, __LINE__);
    Debug::Info("GLSL Version: " + std::string((char*)glslVersion), __FILE__, __LINE__);
}

void Window::setAttributes(int major_, int minor_) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major_);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor_);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    // SDL3: SDL_GL_SetSwapInterval returns bool and sets swap interval (VSync)
    SDL_GL_SetSwapInterval(1);
    glewExperimental = GL_TRUE;
}