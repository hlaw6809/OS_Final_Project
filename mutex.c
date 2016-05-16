#include <mutex.h>
#define MUTEX_ERROR 3

int mutex_trylock (PCB_p raw_pcb, MUTEX_p mutex) {
	if(!mutex->owner || mutex->owner == raw_pcb) {
		return SUCCESS;
	}
	return MUTEX_ERROR;
}

int mutex_lock (PCB_p raw_pcb, MUTEX_p mutex) {
	if(MUTEX_TryLock(raw_pcb, mutex) == 0) {
		mutex->owner = raw_pcb;
		return SUCCESS;
	}
	return MUTEX_ERROR;
}

int mutex_unlock (PCB_p raw_pcb, MUTEX_p mutex) {
	// if owner is this pcb then unlock mutex,
	// set the owner to be next from waiting Queue
	if (mutex->owner == raw_pcb) {
		if (FIFOq_is_empty(mutex->waitingQueue)) {
			mutex->owner = NULL;
		}
		else {
			mutex->owner = FIFOq_dequeue(mutex->waitingQueue);
		}
		return SUCCESS;
	}
	else {
		return MUTEX_ERROR;
	}
}