#ifndef TIMER_H
#define TIMER_H

typedef struct {
	int quantum;
	int time;
} Timer;

typedef Timer * Timer_p;

Timer_p new_timer(int quantum);

int tick_timer(Timer_p timer);

#endif