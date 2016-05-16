#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ReadyQueue.h"

ReadyQueue_p ReadyQueue_Create(){
	ReadyQueue_p Rqueue = malloc(sizeof(ReadyQueue));
	int i;
	for(i = 0; i < 16; i++) {
		FIFOq_p tem = FiFoCreateQueue();
		memcpy(&(Rqueue -> data[i]),tem,sizeof(FIFOq));
		free(tem -> head);
		free(tem -> tail);
		free(tem);
		tem = NULL;
	}
	return Rqueue;
}

void ReadyQueue_Deconstruct(ReadyQueue_p thisQueue){
	free(thisQueue -> data);
	free(thisQueue);
	thisQueue = NULL;
}

int Ready_Enqueue(ReadyQueue_p thisRQueue, PCB_p thisPCB){
	FifoNode_p temp = (FifoNode_p) malloc(sizeof(FifoNode));
	temp -> data = thisPCB;
	temp -> next_node = NULL;
	//FIFOq_p pFifoQueue = &(thisRQueue -> data[thisPCB->priority]);
	Fifo_enqueue(&(thisRQueue -> data[thisPCB->priority]), temp);
	return 0;
}

FifoNode_p Ready_Dequeue(ReadyQueue_p thisRQueue){
	//FIFOq_p temQ = malloc(sizeof(FIFOq));
	int highPrority = 0;
	int max = 0;
	while(Fifo_is_empty(&(thisRQueue -> data[highPrority]))&&max<15){
		highPrority++;
		max++;
	}
	return Fifo_dequeue(&(thisRQueue->data[highPrority]));
}

void ReadyQueue_toString(ReadyQueue_p thisRqueue){
	FIFOq *temQ;
	int i;
	for(i = 0; i <= 15; i++){
		temQ = &(thisRqueue ->data[i]);
		if (!Fifo_is_empty(temQ)) {
			printf("Q%d:Count=%d: ", i, temQ->size);
			FifoNode_p temp = temQ->head;
			printf("P1-");
			int count = 2;
			while (temp->next_node != NULL) {
				temp = temp->next_node;
				printf("P%d-", count);
				count++;
			}
			printf("*\n");
		}
	}
};
