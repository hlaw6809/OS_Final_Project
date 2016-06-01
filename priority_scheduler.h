/*
 * priority_scheduler.h
 * May 13, 2016
 *
 */

#ifndef PRIORITY_SCHEDULER_H_
#define PRIORITY_SCHEDULER_H_

#include "pcb_h.h"
#include "ReadyQueue.h"

void priority_scheduler(FIFOq queue, PCB_p current);
void prevent_starvation(PCB_p current);
void bring_back_priority(ReadyQueue readyQueue, FIFOq queue, PCB_p current);

#endif /* PRIORITY_SCHEDULER_H_ */
