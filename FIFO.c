#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "FIFO.h"


//Enqueue
 void FIFOq_enqueue (FIFOq_p queue, PCB_p pcb) {
 	//create a new node
 	Node* newNode = malloc (sizeof(Node));
 	newNode->pcb = pcb; //assign pcb to new node
 	newNode->next = NULL;

 	if(queue->head == NULL) { //no head
 		queue->head = newNode; //assign the newNode to head
 	} else {
 		queue->tail->next = newNode;
 	}
	
 	queue->tail = newNode;
	queue->size++;
 }

//Dequeue
 PCB_p FIFOq_dequeue (FIFOq_p queue) {

     Node* head = queue->head; //returns the first pcb
     PCB_p pcb;

    if(FIFOq_is_empty(queue)) {
		return NULL;
    } else {
        pcb = head->pcb;
 	      //moves head pointer to the next node
 	    queue->head = head->next;
 	      //decrease size
 	    queue->size--;

 	    //DONT FORGET TO FREE THE MEMORY
 	    free(head);
    }
    return pcb;

 }

//Peek
 PCB_p peek (FIFOq_p queue){
 	PCB_p currPcb = queue->head->pcb;
 	return currPcb;
 }

//is empty
int FIFOq_is_empty(FIFOq_p queue) {

    if(queue->head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

//displays pcb size if there's any queue (Did it to test)
 char * FIFOq_toString (FIFOq_p queue) {
	int count = queue->size;
	char * string = calloc(1000, 1);
	char * nodeString = calloc(600, 1);
	char * pcbString = calloc(10, 1);
 	//if list is empty
 	if(!FIFOq_is_empty(queue)) {
		Node * node = queue->head;
 		int i;
 		for(i = 0; i < count; i++) {
 			if(i > 0) {
				strcat(nodeString, "->");
			}
			sprintf(pcbString, "P%d", i);
			strcat(nodeString, pcbString);
 			node = node->next;
			if (node == NULL) {
				strcat(nodeString, "-*");
			}
 		}
 	}
	char * countString = calloc(1, 400);
	sprintf(countString, "Count=%d: ", count);
	strcat(string, strcat(countString, nodeString));
	
	free(nodeString);
	free(pcbString);
	free(countString);
	return string;
 }

//creates and initialize queue.
FIFOq_p FIFOq_construct() {
	FIFOq_p queue = malloc(sizeof(FIFOq));
	queue->size = 0;
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

void FIFOq_destruct(FIFOq_p queue) {
	Node * node = queue->head;
	while (node != NULL) {
		free(node->pcb);
		Node * toFree = node;
		node = node->next;
		free(toFree);
	}
	free(queue);
}
