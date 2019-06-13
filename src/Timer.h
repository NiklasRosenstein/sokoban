#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

struct Timer {
    uint32_t start_ticks;
    uint32_t paused_ticks;
    bool started;
    bool paused;
};

void Timer_start(struct Timer *timer);

void Timer_stop(struct Timer *timer);

void Timer_pause(struct Timer *timer);

void Timer_resume(struct Timer *timer);

uint32_t Timer_get_ticks(struct Timer *timer);

float Timer_runtime(struct Timer *timer);

#endif
