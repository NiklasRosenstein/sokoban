
#include "Timer.h"

//struct Timer {
//    uint32_t start_ticks;
//    uint32_t paused_ticks;
//    bool started;
//    bool paused;
//};

//static uint32_t start_ticks = 0;
//static uint32_t paused_ticks = 0;
//static bool started = false;
//static bool paused = false;

void Timer_start(struct Timer *timer) {
    timer->started = true;
    timer->paused = false;
    timer->paused_ticks = 0;
    timer->start_ticks = SDL_GetTicks();
}

void Timer_stop(struct Timer *timer) {
    timer->started = false;
    timer->paused = false;
    timer->paused_ticks = 0;
    timer->start_ticks = 0;
}

void Timer_pause(struct Timer *timer) {
    timer->paused = true;
    timer->paused_ticks = SDL_GetTicks() - timer->start_ticks;
    timer->start_ticks = 0;
}

void Timer_resume(struct Timer *timer) {
    timer->paused = false;
    timer->start_ticks = SDL_GetTicks() - timer->paused_ticks;
}

uint32_t Timer_get_ticks(struct Timer *timer) {
    if (!timer->started) {
        return 0;
    }

    uint32_t ticks;

    if (timer->paused) {
        ticks = timer->paused_ticks;
    } else {
        ticks = SDL_GetTicks() - timer->start_ticks;
    }

    return ticks;
}

float Timer_runtime(struct Timer *timer) {
    return Timer_get_ticks(timer) / 1000.0f;
}
