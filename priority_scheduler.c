#include <stdio.h>
#include "priority_scheduler.h"

priority_scheduler(FIFOq queue)
{
	//Create ready queue
	ReadyQueue_p readyQueue = ReadyQueue_Create();

	//Create the following Priority Levels and filled each of them to a certain percentage
	//Priority Level One is filled 0-5%
	FIFOq priorityLvl1 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 0-5% full

	//enqueue priorityLvl1 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl1);

	//Priority Level Two is filled 1-80%
	FIFOq priorityLvl2 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 1-80% full

	//enqueue priorityLvl2 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl2);

	//Priority Level Three is filled 2-10%
	FIFOq priorityLvl3 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 2-10% full

	//enqueue priorityLvl3 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl3);

	//Priority Level Four is filled 3-5%
	FIFOq priorityLvl4 = FIFOq_construct();

	//enqueue PCBs into priorityLvl1 until it's 3-5% full

	//enqueue priorityLvl4 into readyQueue
	Ready_Enqueue(readyQueue, priorityLvl4);
}

prevent_starvation(PCB_p current)
{
	//have start and end time
	int start_time = 0;
	int end_time = 0;

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

				start_time = timer->quantum;
			}

			//keep track of quanta
			end_time = timer->quantum;

			//if a certain amount of quanta is reached
			if (end_time - start_time > 10)
			{
				//after a certain amount of quanta has passed, priority goes to its original amount
				current->priority = current->priority + 1;
			}
		}
	}
}
