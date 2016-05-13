#include <stdlib.h>
#include "mutex.h"
#include "FIFO.h"

Mutex_p mutex_construct() {
	Mutex_p mutex = malloc(sizeof(Mutex));
	mutex->owner = NULL;
	mutex->requesters = FIFOq_construct();
	return mutex;
}

void mutex_destruct(Mutex_p toDestruct) {
	FIFOq_destruct(toDestruct->requesters);
	free(toDestruct);
}

int lock_mutex(Mutex_p toLock, PCB_p requester) {
	if (toLock->owner == NULL) {
		toLock->owner = requester;
		return 1;
	} else {
		FIFOq_enqueue(toLock->requesters, requester);
		return 0;
	}
}

int unlock_mutex(Mutex_p toUnlock, PCB_p owner) {
	if (toUnlock->owner->pid == owner->pid) {
		toUnlock->owner = NULL;
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