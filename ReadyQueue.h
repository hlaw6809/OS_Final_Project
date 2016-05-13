#include "fifo.h"
#ifndef READYQUEUE_H
#define READYQUEUE_H


typedef struct readyqueue{
	FIFOq data[16];
} ReadyQueue;
typedef ReadyQueue * ReadyQueue_p;

ReadyQueue_p ReadyQueue_Create();
void ReadyQueue_Deconstruct(ReadyQueue_p RQueue);
int Ready_Enqueue(ReadyQueue_p theReadyQueue, PCB_p thePCB);
FifoNode_p Ready_Dequeue(ReadyQueue_p theReadyQueue);
void ReadyQueue_toString(ReadyQueue_p theReadyQueue);

#endif
