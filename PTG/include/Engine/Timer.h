#ifndef TIMER_H
#define TIMER_H

#include <SDL3/SDL.h>

class Timer {
public:
	Timer();
	~Timer();

	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;

	void Start();
	void UpdateFrameTicks();
	float GetDeltaTime() const;
	unsigned int GetSleepTime(const unsigned int fps_) const;
	float GetCurrentTicks() const;
    
	static void SetSingleEvent(Uint32 interval, void* param);

private:    
	Uint64 prevTicks;
	Uint64 currentTicks;
	static Uint32 singleEventID;

	// SDL3 timer callback signature: (void* userdata, SDL_TimerID timerID, Uint32 interval)
	static Uint32 SDLCALL callBackFuncion(void* userdata, SDL_TimerID timerID, Uint32 interval);
};

#endif