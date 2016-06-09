// assignment 3
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
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
	if(raw_pcb != NULL) {
		if (raw_pcb->label != NULL) {
			free(raw_pcb->label);	
		}
		free(raw_pcb);
	}
}

// pcb initiation
// in running status, term_count change to 1 and state to running
// regular pcb init with pcb_type type
int PCB_init (PCB_p raw_pcb, enum pcb_type type) {
	time_t now = time(0);
	srand((unsigned) rand());
	// max pc value from 1000 to 3000
	unsigned long r = rand() % (3000 - 1000 + 1) + 1000;
	// assign any pc value below max pc value
	unsigned long r2 = (rand() % r);
	// assign terminate rounds at least 1 times
	unsigned long r3 = (rand() % 2) + 1;
	static int id;
	raw_pcb->pid = id;
	id++;
	raw_pcb->state = ready;
	raw_pcb->priority = 0;
	raw_pcb->max_pc = r;
	raw_pcb->pc = r2;
	raw_pcb->creation = now;
	raw_pcb->terminate = r3;
	raw_pcb->term_count = 0;
	raw_pcb->label = "";
	// set up the trap pc values
	// if the pcb type is normal
	if (type == normal) {
		raw_pcb->type = normal;
		int i;
		for (i=0;i<4;i++) {
			raw_pcb->io1_traps[i] = rand() % r + 1;
		} 
		for (i=0;i<4;i++) {
			raw_pcb->io2_traps[i] = rand() % r + 1;
		}
	}
	else {
		raw_pcb->type = type;
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

int PCB_set_type (PCB_p raw_pcb, enum pcb_type type) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	raw_pcb -> type = type;
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

char* PCB_get_type_str (PCB_p raw_pcb) {
	char *c = malloc(600);
	if(!raw_pcb) {
		sprintf(c,"NO_OBJECT_ERROR");
	} else {
		switch(raw_pcb -> type) {
			case normal: sprintf(c,"Normal"); break;
			case idle: sprintf(c,"Idle"); break;
			case consumer: sprintf(c,"Consumer"); break;
			case producer: sprintf(c,"Producer"); break;
			case deadlock_pair: sprintf(c,"Deadlock Pair"); break;
		}
	}
	return c;
}

/*
char* PCB_get_type_str (PCB_p raw_pcb) {
	if(!raw_pcb) {
		return "NO_OBJECT_ERROR";
	}
	switch(raw_pcb -> type) {
		case normal: return "Normal";
		case idle: return "Idle";
		case consumer: return "Consumer";
		case producer: return "Producer";
		case deadlock_pair: return "Deadlock Pair";
	}
}
enum pcb_type PCB_get_type (PCB_p raw_pcb) {
	if(!raw_pcb) {
		return NO_OBJECT_ERROR;
	}
	return raw_pcb->type;
}
*/

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
	char *c = malloc(600);
	if(!raw_pcb) {
		sprintf(c,"NO_OBJECT_ERROR");
		return c;
	}
	else {
		// print out the contents of pcb
		char *s = malloc(1000);
		strcpy(s, ctime(&(raw_pcb->creation)));
		s[strlen(s) - 1] = '\0';
		char * pcbtype = PCB_get_type_str(raw_pcb);
		sprintf(c,"PID: %lu, Label: %s, Created At: %s, Priority: %u, pcb_type: %s, state: %d, PC: %ld, Max_PC: %ld, terminate: %d ",raw_pcb->pid, raw_pcb->label, s, raw_pcb->priority, pcbtype, raw_pcb->state,raw_pcb->pc,raw_pcb->max_pc,raw_pcb->terminate);
		free(pcbtype);
		free(s);
		return c;
	}
}


