#ifndef MUTEX_H
#define MUTEX_H
#include "pcb_h.h"
#include "FIFO.h"

typedef struct mutex {
	PCB_p owner;
	FIFOq_p waitingQueue;
	int id;
} MUTEX;

typedef MUTEX * MUTEX_p;

// return 3 mutex_error 0 success
MUTEX_p mutex_construct (int id);
void mutex_deconstruct (MUTEX_p mutex);
int mutex_trylock (PCB_p raw_pcb, MUTEX_p raw_mutex);
int mutex_lock (PCB_p raw_pcb, MUTEX_p raw_mutex);
int mutex_unlock (PCB_p raw_pcb, MUTEX_p raw_mutex);

#endif








