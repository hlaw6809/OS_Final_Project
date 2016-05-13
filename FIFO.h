#ifndef FIFO
#define FIFO

#include <stdio.h>
#include <stdlib.h>
#include "pcb_h.h"

/**
 *The node struct:
 * pcb and pointer to point to next node.
 */

 typedef struct Node {
 	PCB_p pcb;
 	struct Node * next;
 } Node;


 /*
  * FIFOq struct
  */

 typedef struct fifoq{
 	Node * head;
 	Node * tail;
 	int size; //size of the FIFOq
 } FIFOq;
 
 typedef FIFOq * FIFOq_p;

 /*
  * FIFOq Methods
  */

 FIFOq_p FIFOq_construct ();
 void FIFOq_destruct (FIFOq_p);
 void FIFOq_enqueue (FIFOq_p, PCB_p);
 PCB_p FIFOq_dequeue (FIFOq_p);
 PCB_p peek (FIFOq_p);
 char * FIFOq_toString (FIFOq_p);
 int FIFOq_is_empty(FIFOq_p);


#endif
