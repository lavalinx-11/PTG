#include <typeinfo>
#include <iostream>
#include <SDL3/SDL.h>
#include "Engine/Timer.h"

Timer::Timer() : prevTicks{0}, currentTicks{0} {}

Timer::~Timer() {}

void Timer::Start() {
    prevTicks = SDL_GetTicks();
    currentTicks = SDL_GetTicks();
}

void Timer::UpdateFrameTicks() {
    prevTicks = currentTicks;
    currentTicks = SDL_GetTicks();
}

float Timer::GetDeltaTime() const {
    // In SDL3, SDL_GetTicks() returns Uint64 (milliseconds).
    // SDL_GetTicksNS() is also available for nanosecond precision.
    return static_cast<float>(currentTicks - prevTicks) / 1000.0f;
}

unsigned int Timer::GetSleepTime(const unsigned int fps_) const {
    unsigned int milliSecsPerFrame = 1000 / fps_;
    if (milliSecsPerFrame == 0) {
       return 0;
    }
    Uint64 current = SDL_GetTicks();
    unsigned int sleepTime = milliSecsPerFrame - static_cast<unsigned int>(current);
    if (sleepTime > milliSecsPerFrame) {
       return milliSecsPerFrame;
    }
    return sleepTime;
}

float Timer::GetCurrentTicks() const {
    return static_cast<float>(currentTicks) / 1000.0f;
}

/// Single event stuff
Uint32 Timer::singleEventID = 0; /// initialize the static member

void Timer::SetSingleEvent(Uint32 interval, void* param) { 
    // In SDL3, timer callbacks use SDL_TimerCallback: SDL_NS_PER_SECOND ticks are passed,
    // or you can pass ms directly using SDL_AddTimer.
    SDL_TimerID id = SDL_AddTimer(interval, callBackFuncion, param);
}

// In SDL3, the timer callback signature changed:
// Return value is Uint32 (ms delay until next trigger; return 0 to cancel),
// and parameters are (void* userdata, SDL_TimerID timerID, Uint32 interval).
Uint32 Timer::callBackFuncion(void* userdata, SDL_TimerID timerID, Uint32 interval) {
    SDL_Event event;
    SDL_zero(event); // Good practice in SDL3 to clear event structs

    event.type = SDL_EVENT_USER;
    event.user.code = 0;
    event.user.data1 = reinterpret_cast<void*>(static_cast<uintptr_t>(singleEventID));
    event.user.data2 = userdata;

    ++singleEventID; /// Inc the ID of each event registered 

    SDL_PushEvent(&event);
    return 0; /// Stop the timer
}