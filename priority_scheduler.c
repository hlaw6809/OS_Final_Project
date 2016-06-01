#include <stdio.h>
#include <stdlib.h>
#include "priority_scheduler.h"

//Max number of PCBs
#define MAX_PCBS 50
//RAND MIN for Priority Level 2 min is 1%
#define RAND_MIN1 1;
//RAND MIN for Priority Level 3 min is 2%
#define RAND_MIN2 1;
//RAND MIN for Priority Level 4 min is 3%
#define RAND_MIN3 2;
//RAND MAX for Priority Level 1 and 4 max is 5%
#define RAND_MAX1 3;
//RAND MAX for Priority Level 2 max is 80%
#define RAND_MAX2 40;
//RAND MAX for Priority Level 3 max is 10%
#define RAND_MAX3 5;

priority_scheduler(FIFOq queue, PCB_p current)
{
	//check every pcb below 0 and if pcb

	int numOfEnqueue = 0;

	//Create ready queue
	ReadyQueue_p readyQueue = ReadyQueue_Create();

	//Create the following Priority Levels and filled each of them to a certain percentage
	//Priority Level One is filled 0-5%
	FIFOq priorityLvl1 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 0-5% full
	numOfEnqueue = rand() % RAND_MAX1;

	//enqueue Priority Level One until certain percentage is filled
	for (int i = 0; i < numOfEnqueue; i++)
	{
		//enqueue PCB
		FIFOq_enqueue(priorityLvl1, current);
	}

	//enqueue priorityLvl1 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl1);

	//Priority Level Two is filled 1-80%
	FIFOq priorityLvl2 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 1-80% full
	numOfEnqueue = rand() % RAND_MAX2 + RAND_MIN1;

	//enqueue Priority Level Two until certain percentage is filled
	for (int i = 0; i < numOfEnqueue; i++)
	{
		//enqueue PCB
		FIFOq_enqueue(priorityLvl2, current);
	}

	//enqueue priorityLvl2 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl2);

	//Priority Level Three is filled 2-10%
	FIFOq priorityLvl3 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 2-10% full
	numOfEnqueue = rand() % RAND_MAX3 + RAND_MIN2;

	//enqueue Priority Level Three until certain percentage is filled
	for (int i = 0; i < numOfEnqueue; i++)
	{
		//enqueue PCB
		FIFOq_enqueue(priorityLvl3, current);
	}

	//enqueue priorityLvl3 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl3);

	//Priority Level Four is filled 3-5%
	FIFOq priorityLvl4 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 1-80% full
	numOfEnqueue = rand() % RAND_MAX1 + RAND_MIN3;

	//enqueue Priority Level One until certain percentage is filled
	for (int i = 0; i < numOfEnqueue; i++)
	{
		//enqueue PCB
		FIFOq_enqueue(priorityLvl4, current);
	}

	//enqueue priorityLvl4 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl4);
}

prevent_starvation(PCB_p current, FIFOq queue)
{
	//have start and end time
	int start_time = 0;
	int end_time = 0;

	//get the first node of the queue
	Node* head = queue->head;

	//make the current pcb the head node
	current = head->pcb;

	//check every PCB every time the loop is restarting
	//if the timer is 0
	if (timer == 0)
	{
		//while the ready queue is not null
		while (readyQueue != NULL)
		{
			//check each PCB
			//if PCB's term count is 0, temporarily boost its priority
			if (current->term_count == 0)
			{
				current->priority = current->priority - 1;

				//switch PCB to a different priority level

				//switch the flag on PCB to true
				current->flag = true;

				//start_time = timer->quantum;
			}

			//go to the next node
			head = head->next;
			current = head->pcb;

			//keep track of quanta
			//end_time = timer->quantum;

			//if a certain amount of quanta is reached
			/*
			if ((end_time - start_time) > 10)
			{
				//priority goes to its original amount
				current->priority = current->priority + 1;
			}*/
		}
	}
}

void bring_back_priority(ReadyQueue readyQueue, FIFOq queue, PCB_p current)
{
	//get the first node of the queue
	Node* head = queue->head;

	//make the current pcb the head node
	current = head->pcb;

	//while the ready queue is not null
	while (readyQueue != NULL)
	{
		//check each PCB
		//if a certain amount of time has passed
		if (current->quantumChk > 10)
		{
			//priority goes to its original amount
			current->priority = current->priority + 1;

			//make quantum check equal to 0
			current->quantumChk = 0;

			//make PCB's flag false
			current->flag = false;
		}

		//go to the next node
		head = head->next;
		current = head->pcb;
	}
}
