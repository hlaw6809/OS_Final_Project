//OS project part 3
//Created by Henry Lawrence, Tina Wang, and Ashton Ohms

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include "pcb_h.h"
#include "FIFO.h"
#include "mutex.h"

#define MRU_COUNT 10

typedef int * IO_p;

enum schedule_type {TIMER, TERMINATION, TRAP, MUTEX_INTERRUPT};

PCB_p runningProcess;
Timer_p timer;
FIFOq_p readyQueue;
FIFOq_p trap1WaitingQueue;
FIFOq_p trap2WaitingQueue;
FIFOq_p terminationQueue;

IO_p io1;
IO_p io2;

Mutex_p pcMutexes[5];
int counts[5];

Mutex_p deadlockMutexes[10];

int tick_IO(IO_p io) {
	(*io)--;
	int interrupted = 0;
	if (*io == 0) {
		interrupted = 1;
		*io = 100;
	}
	return interrupted;
}

void dispatcher() {
	if (FIFOq_is_empty(readyQueue) == 0) {
		runningProcess = FIFOq_dequeue(readyQueue);
		char * pcbString = PCB_toString(runningProcess);
		printf("Now Running: %s\n",pcbString);
		free(pcbString);
		if (runningProcess != NULL) {
			PCB_set_state(runningProcess, running);
		}
	} else {
		printf("Now Running: Idle\n");
		runningProcess = NULL;
	}
}

//Add currently running proccess to ready queue and call dispatcher to dispatch next proccess.
void scheduler(enum schedule_type type) {
	if (type == TIMER || type == MUTEX_INTERRUPT) {
		if (runningProcess != NULL) {
			if (type == MUTEX_INTERRUPT) {
				(runningProcess->pc)--;
			}
			PCB_set_state(runningProcess, ready);
			char * pcbString = PCB_toString(runningProcess);
			printf("Returned to ReadyQueue: %s\n",pcbString);
			free(pcbString);
			FIFOq_enqueue(readyQueue, runningProcess);
		}
	} else if (type == TERMINATION) {
		PCB_set_state(runningProcess, terminated);
		// set up the termination time of runningProcess
		time_t now = time(0);
		runningProcess->termination = now;
		char *s = malloc(1000);
		strcpy(s, ctime(&(runningProcess->termination)));
		s[strlen(s) - 1] = '\0';
		char * pcbString = PCB_toString(runningProcess);
		printf("Terminated at %s: %s\n",s,pcbString);
		free(pcbString);
		free(s);
		FIFOq_enqueue(terminationQueue, runningProcess);
		runningProcess = NULL;
	}
	dispatcher();
}

// check if current process need to be terminated..
bool PCB_check_terminate () {
	if(runningProcess->terminate == 0 || runningProcess->term_count < runningProcess->terminate) {
		return false;
	}
	else{
  		return true;
  	}
}

// trap handler passing the trap service number.
// work very similarly to the timer ISR in terms of taking the running process
// out of the state and put it into the waiting queue
void io_trap_handler(int trapNum) {
	FIFOq_p queue;
	switch(trapNum) {
		case 1: queue = trap1WaitingQueue;
				break;
		case 2: queue = trap2WaitingQueue;
				break;
		default: return;
	}
	// put running process to waiting queue
	PCB_set_state(runningProcess, waiting);
	FIFOq_enqueue(queue, runningProcess);
	char * pcbString = PCB_toString(runningProcess);
	printf("Enqueuing Trap %d Waiting Queue: %s\n",trapNum,pcbString);
	free(pcbString);
	scheduler(TRAP);
}

int isDeadlocked() {
	int i;
	for (i = 0; i < 10; i++) {
		Mutex_p mutex1 = deadlockMutexes[i];
		PCB_p owner1 = mutex1->owner;
		if (owner1 != NULL) {
			int j;
			for (j = 0; j < 5; j++) {
				Mutex_p mutex2 = deadlockMutexes[j];
				PCB_p owner2 = mutex2->owner;
				if (owner2 != NULL && mutex1->id != mutex2->id &&  FIFOq_contains(mutex2->requesters, owner1) && FIFOq_contains(mutex1->requesters, owner2)) {
					printf("DEAD LOCK HAS OCCURED ON MUTEX %d, ENDING PROGRAM\n", mutex1->id);
					printf("Mutex %d Owner %d\n", mutex1->id, owner1->pid);
					printf("Mutex %d owner %d\n", mutex2->id, owner2->pid);
					char * queueString = FIFOq_toString(readyQueue);
					printf("\nQueue: %s\n",mutex1->requesters);
					free(queueString);
					return 1;
				}
			}
		}
	}
	return 0;
}


void makeMutualResourceUserPairs(int canDeadlock) {
	srand(time(NULL));
	int pairId;
	for (pairId = 0; pairId < 5; pairId++) {
		PCB_p pcb1 = PCB_construct();
		PCB_p pcb2 = PCB_construct();
		PCB_init(pcb1, deadlock_pair);
		PCB_init(pcb2, deadlock_pair);	
		pcb1->max_pc = 600;
		pcb2->max_pc = 600;
		pcb1->terminate = 10;
		pcb2->terminate = 10;
		pcb1->pc = 0;
		pcb2->pc = 0;
		Mutex_p mutex1 = mutex_construct();
		Mutex_p mutex2 = mutex_construct();
		int pcb1StartLine = rand() % (pcb1->max_pc - 5);
		int pcb2StartLine = rand() % (pcb2->max_pc - 5);
		
		char * p = malloc(50);
		sprintf(p, "User 1 for resource pair %d", pairId);
		pcb1->label = p;
		p = malloc(50);
		sprintf(p, "User 2 for resource pair %d", pairId);
		pcb2->label = p;
		
		int mutex1Index = pairId * 2;
		int mutex2Index = pairId * 2 + 1;
		
		//Lock mutex 1
		PCB_instruction inst1 = {pcb1StartLine, mutex_lock, mutex1Index};
		pcb1->instructions[0] = inst1;		
		//Lock mutex 2
		PCB_instruction inst2 = {pcb1StartLine + 1, mutex_lock, mutex2Index};
		pcb1->instructions[1] = inst2;
		//Print Message
		PCB_instruction inst3 = {pcb1StartLine + 2, critical, 0};
		pcb1->instructions[2] = inst3;
		//Unlock mutex 2
		PCB_instruction inst4 = {pcb1StartLine + 3, mutex_unlock, mutex2Index};
		pcb1->instructions[3] = inst4;
		//Lock mutex 1
		PCB_instruction inst5 = {pcb1StartLine + 4, mutex_unlock, mutex1Index};
		pcb1->instructions[4] = inst5;
		
		if (canDeadlock == 1) {
			//Lock mutex 2
			PCB_instruction inst6 = {pcb2StartLine, mutex_lock, mutex2Index};
			pcb2->instructions[0] = inst6;
			//Lock mutex 1
			PCB_instruction inst7 = {pcb2StartLine + 1, mutex_lock, mutex1Index};
			pcb2->instructions[1] = inst7;
			//Print Message
			PCB_instruction inst8 = {pcb2StartLine + 2, critical, 0};
			pcb2->instructions[2] = inst8;
			//Unlock mutex 1
			PCB_instruction inst9 = {pcb2StartLine + 3, mutex_unlock, mutex1Index};
			pcb2->instructions[3] = inst9;
			//Lock mutex 2
			PCB_instruction inst10 = {pcb2StartLine + 4, mutex_unlock, mutex2Index};
			pcb2->instructions[4] = inst10;
		} else {
			//Lock mutex 1
			PCB_instruction inst6 = {pcb2StartLine, mutex_lock, mutex1Index};
			pcb2->instructions[0] = inst6;
			//Lock mutex 2
			PCB_instruction inst7 = {pcb2StartLine + 1, mutex_lock, mutex2Index};
			pcb2->instructions[1] = inst7;
			//Print Message
			PCB_instruction inst8 = {pcb2StartLine + 2, critical, 0};
			pcb2->instructions[2] = inst8;
			//Unlock mutex 2
			PCB_instruction inst9 = {pcb2StartLine + 3, mutex_unlock, mutex2Index};
			pcb2->instructions[3] = inst9;
			//Lock mutex 1
			PCB_instruction inst10 = {pcb2StartLine + 4, mutex_unlock, mutex1Index};
			pcb2->instructions[4] = inst10;
		}
		
		char * pcbString = PCB_toString(pcb1);
		printf("%s\n",pcbString);
		free(pcbString);
		
		pcbString = PCB_toString(pcb2);
		printf("%s\n",pcbString);
		free(pcbString);
		
		FIFOq_enqueue(readyQueue, pcb1);
		FIFOq_enqueue(readyQueue, pcb2);
		deadlockMutexes[mutex1Index] = mutex1;
		deadlockMutexes[mutex2Index] = mutex2;
		
	}
}

void createPCPairs() {
	int i;
	for (i = 0; i < 5; i++) {
		pcMutexes[i] = mutex_construct();
		
		PCB_p consumerPCB = PCB_construct();
		PCB_init(consumerPCB, consumer);
		consumerPCB->terminate = 10;
		int pcbStartLine = rand() % (consumerPCB->max_pc - 5);
		//Lock Mutex
		PCB_instruction inst1 = {pcbStartLine, mutex_lock, i};
		consumerPCB->instructions[0] = inst1;
		//Critical
		PCB_instruction inst2 = {pcbStartLine + 1, critical, i};
		consumerPCB->instructions[1] = inst2;
		//Unlock Mutex
		PCB_instruction inst3 = {pcbStartLine + 2, mutex_unlock, i};
		consumerPCB->instructions[2] = inst3;
		char * pcbString = PCB_toString(consumerPCB);
		printf("%s\n",pcbString);
		free(pcbString);
		FIFOq_enqueue(readyQueue,consumerPCB);
		
		PCB_p producerPCB = PCB_construct();
		PCB_init(producerPCB, producer);
		producerPCB->terminate = 10;
		pcbStartLine = rand() % (producerPCB->max_pc - 5);
		//Lock Mutex
		PCB_instruction inst4 = {pcbStartLine, mutex_lock, i};
		producerPCB->instructions[0] = inst4;
		//Critical
		PCB_instruction inst5 = {pcbStartLine + 1, critical, i};
		producerPCB->instructions[1] = inst5;
		//Unlock Mutex
		PCB_instruction inst6 = {pcbStartLine + 2, mutex_unlock, i};
		producerPCB->instructions[2] = inst6;
		pcbString = PCB_toString(producerPCB);
		printf("%s\n",pcbString);
		free(pcbString);
		FIFOq_enqueue(readyQueue, producerPCB);
	}
	
}

void initialize() {
	srand(time(NULL));
	io1 = malloc(sizeof(int));
	io2 = malloc(sizeof(int));
	// Randomize the IO timer values
	*io1 = rand() % 100 + 300;
	*io2 = rand() % 100 + 300;
	printf("IO1 Time: %d, IO2 Time: %d\n", *io1, *io2);
	
	//Construct Queues
	readyQueue = FIFOq_construct();
	terminationQueue = FIFOq_construct();
	trap1WaitingQueue = FIFOq_construct();
	trap2WaitingQueue = FIFOq_construct();
	
	//Make Mutex Using PCBS
	makeMutualResourceUserPairs(0);
	createPCPairs();

	int i;
	for (i=0;i<2;i++) {
		PCB_p pcb = PCB_construct();
		PCB_init(pcb, normal);
		char * pcbString = PCB_toString(pcb);
		printf("%s\n",pcbString);
		free(pcbString);
		printf("I/O_1 Traps Values ");
		int j;
		for (j=0;j<4;j++) {
			printf("%d ", pcb->io1_traps[j]);
		}
		printf("\n%s","I/O_2 Traps Values ");
		for (j=0;j<4;j++) {
			printf("%d ", pcb->io2_traps[j]);
		}
		FIFOq_enqueue(readyQueue,pcb);
		char * queueString = FIFOq_toString(readyQueue);
		printf("\nReady Queue: %s\n",queueString);
		free(queueString);
		printf("-----------------\n");
	}
	runningProcess = FIFOq_dequeue(readyQueue);
	char * pcbString = PCB_toString(runningProcess);
	printf("Now Running: %s\n",pcbString);
	free(pcbString);
}


int main(int argc, char* argv[]) {
	initialize();
	timer = new_timer(300);
	int x = 10;
	printf("Starting Simulation\n");
	while(runningProcess != NULL 
		|| FIFOq_is_empty(readyQueue) == 0 
		|| FIFOq_is_empty(trap1WaitingQueue) == 0 
		|| FIFOq_is_empty(trap2WaitingQueue) == 0) {
			
		if (runningProcess != NULL) {
			(runningProcess->pc)++;
			//Check For max PC
			if (runningProcess->pc > runningProcess->max_pc) {
				runningProcess->pc = 0;
				(runningProcess->term_count)++;
				if (PCB_check_terminate()) {
					// if readyQueue is Empty
					scheduler(TERMINATION);
				}
			//Handle normal PCB's with IO trap requests
			} else if (runningProcess->type == normal) {
				int index;
				for (index = 0; index < 4; index++) {
					if (runningProcess->pc == runningProcess->io1_traps[index]) {
						//Handle I/O 1 trap
						io_trap_handler(1);
						break;
					} else if (runningProcess->pc == runningProcess->io2_traps[index]) {
						//Handle I/O 2 trap
						io_trap_handler(2);
						break;
					}
				}
			
			} else if (runningProcess->type == deadlock_pair || runningProcess->type == consumer || runningProcess->type == producer) {
				int index;
				//check for mutex instructions
				for (index = 0; index < 5; index++) {
					int result;
					PCB_instruction inst = runningProcess->instructions[index];
					if (runningProcess->pc == inst.line) {
						Mutex_p mutex;
						if (runningProcess->type == deadlock_pair) {
							mutex = deadlockMutexes[inst.mutexId];
						} else if (runningProcess->type == consumer || runningProcess->type == producer) {
							mutex = pcMutexes[inst.mutexId];
						}
						switch (inst.type) {
							case mutex_lock :
								printf("PID %d: requested lock on mutex %d - ", runningProcess->pid, mutex->id);
								result = lock_mutex(mutex, runningProcess);
								if (result == 1) {
									printf("succeeded\n");
								} else {
									printf("blocked by PID %d\n", mutex->owner->pid);
									if (isDeadlocked() == 1) {
										printf("ENDING PROGRAM");
										return 0;
									}
									scheduler(MUTEX_INTERRUPT);
								}
								break;
								
							case mutex_unlock :
								result = unlock_mutex(mutex, runningProcess);
								if (result == 1) {
									printf("Unlock Mutex %d\n", inst.mutexId);
								} else {
									printf("Did Not Unlock Mutex %d\n", inst.mutexId);
								}
								break;
								
							case try_lock_call :
								result = try_lock(mutex);
								break;
								
							case critical :
								if (runningProcess->type == deadlock_pair) {
									printf("Deadlock Critical Section For PCB %s\n", runningProcess->label);
								} else if (runningProcess->type == consumer) {
									printf("Consumer prints out count value : %d\n",counts[inst.mutexId]);
								} else if (runningProcess->type == producer) {
									counts[inst.mutexId] = counts[inst.mutexId] + 1;
									printf("Count increased by producer: %d\n",counts[inst.mutexId]);
								}
								break;
								
							default: 
								printf("Unrecognized Mutex Instruction\n");
						}
					}
				}
			}
		}
		
		//Tick Timer and I/O timers
		if (tick_timer(timer) == 1) {
			scheduler(TIMER);
		} else if (tick_IO(io1) == 1) {
			if (FIFOq_is_empty(trap1WaitingQueue) == 0) {
				PCB_p pcb = FIFOq_dequeue(trap1WaitingQueue);
				PCB_set_state(pcb, ready);
				FIFOq_enqueue(readyQueue, pcb);
				if (runningProcess == NULL) {
					dispatcher();
				}
			}
			*io1 = rand() % 100 + 300; // Reset random IO timer value
		} else if (tick_IO(io2) == 1) {
			if (FIFOq_is_empty(trap2WaitingQueue) == 0) {
				PCB_p pcb = FIFOq_dequeue(trap2WaitingQueue);
				PCB_set_state(pcb, ready);
				FIFOq_enqueue(readyQueue, pcb);
				if (runningProcess == NULL) {
					dispatcher();
				}
			}
			*io2 = rand() % 100 + 300; // Reset random IO timer value
		}
	}
	PCB_deconstruct(runningProcess);
	FIFOq_destruct(readyQueue);
	FIFOq_destruct(trap1WaitingQueue);
	FIFOq_destruct(trap2WaitingQueue);
	FIFOq_destruct(terminationQueue);
	printf("ENDING PROGRAM \n");
	return 0;
}
