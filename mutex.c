#include <stdlib.h>
#include "mutex.h"
#include "FIFO.h"

Mutex_p mutex_construct() {
	static int id;
	Mutex_p mutex = calloc(1, sizeof(Mutex));
	mutex->owner = NULL;
	mutex->requesters = FIFOq_construct();
	mutex->id = id;
	id++;
	return mutex;
}

void mutex_destruct(Mutex_p toDestruct) {
	FIFOq_destruct(toDestruct->requesters);
	free(toDestruct);
}

int lock_mutex(Mutex_p toLock, PCB_p requester) {
	if (toLock->owner == NULL || requester->pid == toLock->owner->pid) {
		toLock->owner = requester;
		return 1;
	} else {
		if (FIFOq_contains(toLock->requesters, requester) == 0) {
			FIFOq_enqueue(toLock->requesters, requester);	
		}
		return 0;
	}
}

int unlock_mutex(Mutex_p toUnlock, PCB_p owner) {
	if (toUnlock->owner->pid == owner->pid) {
		if (FIFOq_is_empty(toUnlock->requesters)) {
			toUnlock->owner = NULL;
		}
		else {
			toUnlock->owner = FIFOq_dequeue(toUnlock->requesters);
			printf("PID 0x%lu: passed mutex%d ownership to PID 0x%lu \n",owner->pid,toUnlock->id,toUnlock->owner->pid);
		}
		return 1;
	} else {
		printf("Thread can not unluck a mutex it does not own\n");
		return 0;
	}
}

int try_lock(Mutex_p toTry) {
	if (toTry->owner == NULL) {
		return 1;
	} else {
		return 0;
	}
}