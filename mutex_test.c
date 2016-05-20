#include <stdbool.h>
#include <stdlib.h>
#include "timer.h"
#include "pcb_h.h"
#include "FIFO.h"
#include "timer.c"
#include "pcb.c"
#include "FIFO.c"
#include "mutex_h.h"
#include "mutex.c"

MUTEX_p mutex1;

FIFOq_p readyQueue;
FIFOq_p waitingQueue;

PCB_p runningProcess;

int main(int argc, char* argv[]) {
	// constructors
	readyQueue = FIFOq_construct();
	waitingQueue = FIFOq_construct();
	mutex1 = mutex_construct(1);
	// producer
	PCB_p pcb = PCB_construct();
	PCB_init(pcb,producer);
	PCB_set_pid(pcb,1);
	char * pcbString = PCB_toString(pcb);
	printf("%s\n",pcbString);
	free(pcbString);
	FIFOq_enqueue(readyQueue,pcb);
	// consumer
	PCB_p pcb2 = PCB_construct();
	PCB_init(pcb2,consumer);
	PCB_set_pid(pcb2,2);
	pcbString = PCB_toString(pcb2);
	printf("%s\n",pcbString);
	free(pcbString);
	FIFOq_enqueue(readyQueue,pcb2);
	// test pcb3
	// PCB_p pcb3 = PCB_construct();
	// PCB_init(pcb3,consumer);
	// PCB_set_pid(pcb3,3);
	// FIFOq_enqueue(mutex1->waitingQueue,pcb3);
	int count = 1;
	while (FIFOq_is_empty(readyQueue) == 0) {
		runningProcess = FIFOq_dequeue(readyQueue);
		if (runningProcess != NULL && runningProcess->type == 3) {
			if(mutex_lock (runningProcess, mutex1) == 0) {
				count = count + 1;
				printf("Count increased by producer: %d\n",count);
				mutex_unlock (runningProcess, mutex1);
			}
			else {
				// FIFOq_enqueue(readyQueue,runningProcess);
			}
		}
		// consumer
		else if (runningProcess != NULL && runningProcess->type == 2) {
			if(mutex_lock (runningProcess, mutex1) == 0) {
				printf("Consumer prints out count value : %d\n",count);
				mutex_unlock (runningProcess, mutex1);
			}
			else {
				// let it in ready queue?
			}
		}
	}

}