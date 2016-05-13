// assignment 3
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pcb_h.h"
#define SUCCESS 0
#define NO_OBJECT_ERROR 1
#define ALREADY_TERMINATED 2

// constructor and deconstructor
PCB_p PCB_construct (void) {
	PCB_p raw_pcb = (PCB_p) malloc(sizeof(PCB));
	return raw_pcb;
}

void PCB_deconstruct(PCB_p raw_pcb) {
	if(!raw_pcb) {
		free(raw_pcb);
	}
}

// pcb initiation
// in running status, term_count change to 1 and state to running
int PCB_init (PCB_p raw_pcb) {
	time_t now = time(0);
	srand((unsigned) rand());
	// max pc value from 1000 to 3000
	unsigned long r = rand() % (3000 - 1000 + 1) + 1000;
	// assign any pc value below max pc value
	unsigned long r2 = (rand() % r);
	// assign terminate rounds at least 1 times
	unsigned long r3 = (rand() % 2) + 1;
	raw_pcb->state = ready;
	raw_pcb->priority = 0;
	raw_pcb->max_pc = r;
	raw_pcb->pc = r2;
	raw_pcb->creation = now;
	raw_pcb->terminate = r3;
	raw_pcb->term_count = 0;
	// set up the trap pc values
	int i;
	for (i=0;i<4;i++) {
		raw_pcb->io1_traps[i] = rand() % r + 1;
	} 
	for (i=0;i<4;i++) {
		raw_pcb->io2_traps[i] = rand() % r + 1;
	}
	return SUCCESS;
}

int PCB_set_pid (PCB_p raw_pcb, unsigned long pid) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	raw_pcb -> pid = pid;
	return SUCCESS;
}

unsigned long PCB_get_pid (PCB_p raw_pcb) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	return raw_pcb -> pid;
}

int PCB_set_state (PCB_p raw_pcb, enum state_type state) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	raw_pcb -> state = state;
	return SUCCESS;
}

enum state_type PCB_get_state (PCB_p raw_pcb) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	return raw_pcb -> state;
}

int PCB_set_PC (PCB_p raw_pcb, unsigned long pc) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	raw_pcb -> pc = pc;
	return SUCCESS;
}

unsigned long PCB_get_PC (PCB_p raw_pcb) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	return raw_pcb -> state;
}

// set the PCB's state to be terminated and termination time
int PCB_terminate (PCB_p raw_pcb) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	else if (raw_pcb -> state == terminated) {
		return ALREADY_TERMINATED;
	}
	time_t now = time(0);
	raw_pcb->state = terminated;
	raw_pcb->termination = now;
	return SUCCESS;
}

char * PCB_toString (PCB_p raw_pcb) {
	char *c = malloc(500);
	if(!raw_pcb) {
		sprintf(c,"NO_OBJECT_ERROR");
		return c;
	}
	else {
		// print out the contents of pcb
		char *s;
		s = ctime(&(raw_pcb->creation));
		sprintf(c,"PID: 0x%lu, Created At: %s, Priority: 0x%u, state: %d, PC: 0x%ld, Max_PC: 0x%ld, terminate: %d ",raw_pcb->pid,s,raw_pcb->priority,raw_pcb->state,raw_pcb->pc,raw_pcb->max_pc,raw_pcb->terminate);
		return c;
	}
}


