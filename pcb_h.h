// assignment 3
#ifndef PCB_H
#define PCB_H
#include <time.h>

enum state_type {new_state, ready, running, interrupted, waiting, terminated};
// idle pcbs are the intensive pcbs
enum pcb_type {normal, idle, consumer, producer};

enum instruction_type {mutex_lock, mutex_unlock, try_lock_call, print};

typedef struct pcbInst {
	int line;
	enum instruction_type type;
	union {
		int mutexId;
		char * printMessage;
	};
} PCB_instruction;

typedef struct pcb {
	char * label;
	unsigned long pid; // process PID #, a unique number
	enum state_type state;
	unsigned short priority; // priorities 0 = highest, 15 = lowest
	unsigned long pc; // holds the current pc value when preempted
	unsigned long max_pc; // max pc value
	time_t creation; // time get created
	time_t termination; // time get terminated
	unsigned int terminate; // 0 or n the #rounds to be terminated..
	unsigned int term_count; // counter to keep track of how many times process has passed its maxpc value
	int io1_traps[4]; // 4 pc values for trap calls from io1
	int io2_traps[4]; // 4 pc values for trap calls from io2
	PCB_instruction * instructions[5];
	enum pcb_type type;
} PCB;

typedef PCB * PCB_p;

PCB_p PCB_construct (void);
void PCB_deconstruct (PCB_p); // deallocates pcb from the heap

/* initiation and print out
* 1. set the priority to be all 0s
* 2. set all the max pc to be same number 2345
* 3. term_count = 0, initiate the trap1 and trap2 
* 4. set up the creation time by current time
*/
int PCB_init (PCB_p,enum pcb_type type); //sets default values for member data
char * PCB_toString (PCB_p); // returns a string reprersenting the contents of the pcb

// accessors
unsigned long PCB_get_pid (PCB_p); // returns pid of the process
enum state_type PCB_get_state (PCB_p); // get the state of pcb
unsigned long PCB_get_PC (PCB_p); // get the pc value of pcb
char* PCB_get_type_str (PCB_p); // get the type of the pcb
enum pcb_type PCB_get_type (PCB_p);

// mutators
int PCB_set_pid (PCB_p, unsigned long pid);
int PCB_set_state (PCB_p, enum state_type state);
int PCB_set_PC (PCB_p, unsigned long pc);
int PCB_terminate(PCB_p); // pcb terminated and termination set to current time
int PCB_set_type (PCB_p, enum pcb_type type); // set the type of the pcb

#endif
