#ifndef MUTEX
#define MUTEX
#include "pcb_h.h"
#include "FIFO.h"

typedef struct {
	PCB_p owner;
	FIFOq_p requesters;
	int id;
} Mutex;

typedef Mutex * Mutex_p;

Mutex_p mutex_construct();

void mutex_destruct(Mutex_p toDestruct);

int lock_mutex(Mutex_p toLock, PCB_p requester);

int unlock_mutex(Mutex_p toUnlock, PCB_p owner);

int try_lock(Mutex_p toTry);

#endif