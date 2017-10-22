/******************************************************************************
	QUEUE ADT
		Type definitions and function prototypes for the queue ADT
		Written by: Iain S. Davis
		Date:		11 Jun 2013
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
//Global Type Definitions///////////////////////////////////////////////////////

typedef struct queue_node
{
	void *dataPtr;
	struct queue_node *next;
}QUEUE_NODE;

typedef struct
{
	QUEUE_NODE *front;
	QUEUE_NODE *rear;
	int count;
}QUEUE;


//Prototype Declarations////////////////////////////////////////////////////////
QUEUE *createQueue  (void);
QUEUE *destroyQueue (QUEUE *queue);
void  flushQueue    (QUEUE *queue);

int			dequeue			(QUEUE *queue, void **itemPtr);
int			enqueue			(QUEUE *queue, void *itemPtr);
int			queueFront		(QUEUE *queue, void **itemPtr);
int			queueRear		(QUEUE *queue, void **itemPtr);
int			queueCount		(QUEUE *queue);
	
int			emptyQueue		(QUEUE *queue);
int			fullQueue		(QUEUE *queue);

