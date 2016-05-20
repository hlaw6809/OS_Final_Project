#include "mutex_h.h"
#define MUTEX_ERROR 3

MUTEX_p mutex_construct (int id) {
	MUTEX_p mutex = malloc(sizeof(mutex));
	mutex->waitingQueue = malloc(sizeof(FIFOq));
	mutex->waitingQueue = FIFOq_construct();
	mutex->owner = malloc (sizeof(PCB));
	mutex->owner = NULL;
	mutex->id = id;
	return mutex;
}

void mutex_deconstruct(MUTEX_p mutex) {
	if(!mutex) {
		free(mutex);
	}
}

int mutex_trylock (PCB_p raw_pcb, MUTEX_p mutex) {
	if(mutex->owner == NULL || mutex->owner == raw_pcb) {
		return 0;
	}
	else {
		printf("PID 0x%lu: requested lock on mutex %d - blocked by PID 0x%lu \n",raw_pcb->pid,mutex->id,mutex->owner->pid);
		return 3;
	}
}

int mutex_lock (PCB_p raw_pcb, MUTEX_p mutex) {
	if (mutex_trylock(raw_pcb, mutex) == 0) {
		mutex->owner = raw_pcb;
		printf("PID 0x%lu: requested lock on mutex%d - success \n",raw_pcb->pid,mutex->id);
		return 0;
	}
	else {
		FIFOq_enqueue(mutex->waitingQueue,raw_pcb);
		return 3;
	}
	
}

int mutex_unlock (PCB_p raw_pcb, MUTEX_p mutex) {
	// if owner is this pcb then unlock mutex,
	// set the owner to be next from waiting Queue
	if (mutex->owner == raw_pcb) {
		printf("PID 0x%lu: requested unlock on mutex%d - success \n",raw_pcb->pid,mutex->id);
		if (FIFOq_is_empty(mutex->waitingQueue)) {
			mutex->owner = NULL;
		}
		else {
			mutex->owner = FIFOq_dequeue(mutex->waitingQueue);
			printf("PID 0x%lu: passed mutex%d ownership to PID 0x%lu \n",raw_pcb->pid,mutex->id,mutex->owner->pid);
		}
		return 0;
	}
	else {
		return 3;
	}
}
