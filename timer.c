#include <stdbool.h>
#include "timer.h"
#include <stdlib.h>

Timer_p new_timer(int quantum) {
	Timer_p timer = malloc(sizeof(Timer));
	timer->quantum = quantum;
	timer->time = quantum;
	return timer;
}

int tick_timer(Timer_p timer) {
	timer->time--;
	if (timer->time == 0) {
		timer->time = timer->quantum;
		return 1;
	} else {
		return 0;
	}
}