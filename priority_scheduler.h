/*
 * priority_scheduler.h
 *
 *  Created on: May 13, 2016
 *      Author: Christine
 */

#ifndef PRIORITY_SCHEDULER_H_
#define PRIORITY_SCHEDULER_H_

#include "pcb_h.h"
#include "ReadyQueue.h"

void priority_scheduler(FIFOq queue);
void prevent_starvation(PCB_p current);

#endif /* PRIORITY_SCHEDULER_H_ */
