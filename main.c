//OS project part 3
//Created by Henry Lawrence, Tina Wang, and Ashton Ohms

#include <stdbool.h>
#include <stdlib.h>
#include "timer.h"
#include "pcb_h.h"
#include "FIFO.h"
#include "mutex.h"

#define MRU_COUNT 10

typedef int * IO_p;

enum schedule_type {TIMER, TERMINATION, TRAP};

PCB_p runningProcess;
Timer_p timer;
FIFOq_p readyQueue;
FIFOq_p trap1WaitingQueue;
FIFOq_p trap2WaitingQueue;
FIFOq_p terminationQueue;

IO_p io1;
IO_p io2;

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
		printf("Now Running: NULL\n");
		runningProcess = NULL;
	}
}

//Add currently running proccess to ready queue and call dispatcher to dispatch next proccess.
void scheduler(enum schedule_type type) {
	if (type == TIMER) {
		if (runningProcess != NULL) {
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
		char *s;
		s = ctime(&(runningProcess->termination));
		char * pcbString = PCB_toString(runningProcess);
		printf("Terminated at %s: %s\n",s,pcbString);
		free(pcbString);
		FIFOq_enqueue(terminationQueue, runningProcess);
		runningProcess = NULL;
	}
	dispatcher();
}

// check if current process need to be terminated..
bool PCB_check_terminate () {
	if(runningProcess->term_count < runningProcess->terminate) {
		return false;
	}
	// runningProcess->term_count >= runningProcess -> terminate
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






void makeMutualResourceUserPair(int canDeadlock) {
	srand(time(NULL));
	static int pairId;
	PCB_p pcb1 = PCB_construct();
	PCB_p pcb2 = PCB_construct();
	PCB_init(pcb1, consumer);
	PCB_init(pcb2, producer);
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
	pairId++;
	
	//Lock mutex 1
	PCB_instruction inst1 = {pcb1StartLine, mutex_lock, mutex1->id};
	pcb1->instructions[0] = &inst1;
	
	//Lock mutex 2
	PCB_instruction inst2 = {pcb1StartLine + 1, mutex_lock, mutex2->id};
	pcb1->instructions[1] = &inst2;
	
	//Print Message
	p = malloc(50);
	sprintf(p, "PCB %d has locked mutexes %d and %d\n", pcb1->pid, mutex1->id, mutex2->id);
	PCB_instruction inst3 = {pcb1StartLine + 2, print, 0};
	inst3.printMessage = p;
	pcb1->instructions[2] = &inst3;
	
	//Unlock mutex 2
	PCB_instruction inst4 = {pcb1StartLine + 3, mutex_unlock, mutex2->id};
	pcb1->instructions[3] = &inst4;
	
	//Lock mutex 1
	PCB_instruction inst5 = {pcb1StartLine + 4, mutex_unlock, mutex1->id};
	pcb1->instructions[4] = &inst5;
	
	if (canDeadlock) {
		//Lock mutex 2
		PCB_instruction inst6 = {pcb2StartLine, mutex_lock, mutex2->id};
		pcb2->instructions[0] = inst6;
		
		//Lock mutex 1
		PCB_instruction inst7 = {pcb2StartLine + 1, mutex_lock, mutex1->id};
		pcb2->instructions[1] = inst7;
		
		//Print Message
		p = malloc(50);
		sprintf(p, "PCB %d has locked mutexes %d and %d\n", pcb2->pid, mutex2->id, mutex1->id);
		PCB_instruction inst8 = {pcb2StartLine + 2, print, 0};
		inst8.printMessage = p;
		pcb2->instructions[2] = inst8;
		
		//Unlock mutex 1
		PCB_instruction inst9 = {pcb2StartLine + 3, mutex_unlock, mutex1->id};
		pcb2->instructions[3] = inst9;
		
		//Lock mutex 2
		PCB_instruction inst10 = {pcb2StartLine + 4, mutex_unlock, mutex2->id};
		pcb2->instructions[4] = inst10;
	} else {
		//Lock mutex 1
		PCB_instruction inst6 = {pcb2StartLine, mutex_lock, mutex1->id};
		pcb2->instructions[0] = inst6;
		
		//Lock mutex 2
		PCB_instruction inst7 = {pcb2StartLine + 1, mutex_lock, mutex2->id};
		pcb2->instructions[1] = inst7;
		
		//Print Message
		p = malloc(50);
		sprintf(p, "PCB %d has locked mutexes %d and %d\n", pcb2->pid, mutex1->id, mutex2->id);
		PCB_instruction inst8 = {pcb2StartLine + 2, print, 0};
		inst8.printMessage = p;
		pcb2->instructions[2] = inst8;
		
		//Unlock mutex 2
		PCB_instruction inst9 = {pcb2StartLine + 3, mutex_unlock, mutex2->id};
		pcb2->instructions[3] = inst9;
		
		//Lock mutex 1
		PCB_instruction inst10 = {pcb2StartLine + 4, mutex_unlock, mutex1->id};
		pcb2->instructions[4] = inst10;
	}
	
	char * pcbString = PCB_toString(pcb1);
	printf("%s\n",pcbString);
	free(pcbString);
		
	pcbString = PCB_toString(pcb2);
	printf("%s\n",pcbString);
	free(pcbString);
	
	FIFOq_enqueue(readyQueue,pcb1);
	FIFOq_enqueue(readyQueue,pcb2);
}







void initialize() {
	srand(time(NULL));
	io1 = malloc(sizeof(int));
	io2 = malloc(sizeof(int));
	// Randomize the IO timer values
	*io1 = rand() % 100 + 300;
	*io2 = rand() % 100 + 300;
	printf("IO1 Time: %d, IO2 Time: %d\n", *io1, *io2);
	// initiate 2 pcbs as testing
	readyQueue = FIFOq_construct();
	terminationQueue = FIFOq_construct();
	trap1WaitingQueue = FIFOq_construct();
	trap2WaitingQueue = FIFOq_construct();
	int i;
	for (i=0;i<1;i++) {
		PCB_p pcb = PCB_construct();
		PCB_init(pcb, normal);
		PCB_set_pid(pcb,i);
		char * pcbString = PCB_toString(pcb);
		printf("%s\n",pcbString);
		free(pcbString);
		printf("%s","I/O_1 Traps Values ");
		int j;
		for (j=0;j<4;j++) {
			printf("%d ", pcb->io1_traps[j]);
		}
		//printf("\n%s","I/O_2 Traps Values ");
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
	//makeMutualResourceUserPair(0);
	timer = new_timer(300);
	int x = 10;
	Mutex_p mutex = mutex_construct();
	while(runningProcess!=NULL 
		|| FIFOq_is_empty(readyQueue) == 0 
		|| FIFOq_is_empty(trap1WaitingQueue) == 0 
		|| FIFOq_is_empty(trap2WaitingQueue) == 0) {
		if (runningProcess != NULL) {
			(runningProcess->pc)++;
			if (runningProcess->pc >= runningProcess->max_pc) {
				runningProcess->pc = 0;
				(runningProcess->term_count)++;
				if (PCB_check_terminate()) {
					// if readyQueue is Empty
					scheduler(TERMINATION);
				}
			}
			if (runningProcess != NULL) {
				int index;
				//Check for I/O interrupts
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
				//check for mutex instructions
				for (index = 0; index < 5; index++) {
					int locked;
					PCB_instruction inst = runningProcess->instructions[index];
					if (runningProcess->pc == inst->line) {
						switch (inst->type) {
							mutex_lock : 
								locked = lock_mutex(mutex, runningProcess);
								if (!locked) {
									printf("PCB %d could not lock mutex %d\n", runningProcess->pid, mutex->id);
									scheduler(TIMER);
								} else {
									printf("PCB %d could locked mutex %d\n", runningProcess->pid, mutex->id);
								}
						}
					}
				}
			}
		}
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
