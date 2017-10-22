/***************************************************************************
	QUEUE_ADT function definitions
		Written by: Iain S. Davis
		Date:		11 Jun 2013
****************************************************************************/
#include "queue_ADT.h"

/****** createQueue*********************************************************
	Allocates memory for a queue head node from dyamic memory and returns its
	address to the caller
		PRE		nothing
		POST	head has been allocated and initialized
		RETURN	head if successful, NULL if overflow
****************************************************************************/
QUEUE *createQueue (void)
{
//Local Declarations
	QUEUE *queue;

//Statements
	queue = (QUEUE*) malloc(sizeof (QUEUE));

	if(queue)  {
		queue->front = NULL;
		queue->rear = NULL;
		queue->count = 0;
	}

	return queue;
}//createQueue


/****** enqueue ************************************************************
	This algorithm inserts data into a queue
		PRE		queue has been created
		POST	data have been inserted
		RETURN  true (1) if successful, false (0) if overflow
****************************************************************************/
int enqueue (QUEUE *queue, void *itemPtr)
{
//Local Declarations
	QUEUE_NODE *newPtr;

//Statements
	if (!(newPtr = (QUEUE_NODE*) malloc(sizeof(QUEUE_NODE))))  {
		return 0;
	}

	newPtr->dataPtr = itemPtr;
	newPtr->next = NULL;

	if (queue->count == 0)  {
		//Inserting into empty queue
		queue->front = newPtr;
	}
	else  {
		queue->rear->next = newPtr;
	}

	(queue->count)++;
	queue->rear = newPtr;

	return 1;
}//enqueue


/****** dequeue ************************************************************
	This function deletes a node from the queue
		PRE		queue has been created
		POST	Data pointer to queue front returned and front element
				deleted and recycled.
		RETURN	true (1) if successful; false (0) if underflow
****************************************************************************/
int dequeue (QUEUE *queue, void **itemPtr)
{
//Local Declarations
	QUEUE_NODE *deleteLoc;

//Statements
	if (!queue->count)  {
		return 0;
	}

	*itemPtr = queue->front->dataPtr;
	deleteLoc = queue->front;
	if(queue->count == 1)  {
		//Deleting only item in queue
		queue->rear = queue->front = NULL;
	}
	else  {
		queue->front = queue->front->next;
	}

	(queue->count)--;

	free(deleteLoc);

	return 1;
}//dequeue


/****** queueFront *********************************************************
	This function retrieves data at front of queue without changing the 
	queue contents.
		PRE		queue is pointer to an initialized queue
		POSt	itemPtr passed back to the caller
		RETURN	true (1) if successful; false (0) if underflow
****************************************************************************/
int queueFront (QUEUE *queue, void **itemPtr)
{
//Statements
	if (!queue->count)  {
		return 0;
	}
	else  {
		*itemPtr = queue->front->dataPtr;
		return 1;
	}
}//queueFront


/****** queueRear **********************************************************
	Retrieves data at the rear of the queue without changing the queue
	contents.
		PRE		queue is pointer to initialized queue
		POST	Data passed back to caller
		RETURN	true if successful; false if underflow
****************************************************************************/
int queueRear(QUEUE *queue, void **itemPtr)
{
//Statements
	if (!queue->count)  {
		return 0;
	}
	else  {
		*itemPtr = queue->rear->dataPtr;
		return 1;
	}
}//queueRear


/****** emptyQueue *********************************************************
	Function checks to see if queue is empty.
		PRE		queue is a pointer ot a queue head node
		RETURN	true if empty; false if queue has data
****************************************************************************/
int emptyQueue (QUEUE *queue)
{
//Statements
	return (queue->count == 0);
}//emptyQueue


/****** fullQueue **********************************************************
	This function checks to see if queue is full. It is full if memory cannot
	be allocated for next node.
		PRE		queue is a pointer to a queue head node
		RETURN	true if full; false if room for a node
****************************************************************************/
int fullQueue (QUEUE *queue)
{
//Local Declarations
	QUEUE_NODE *temp;

//Statements
	temp = (QUEUE_NODE*)malloc(sizeof(*(queue->rear)));
	if (temp)  {
		free(temp);
		return 1;
	}

	return 0;
}//fullQueue


/****** queueCount *********************************************************
	Returns the number of elements in the queue.
		PRE		queue is pointer to the queue head node
		RETURN	queue count
****************************************************************************/
int queueCount(QUEUE *queue)
{
//Statements
	return queue->count;
}//queueCount


/****** destroyQueue *******************************************************
	Deletes all data from a queue and recycles its memory, then deletes & 
	recycles queue head pointer.
		PRE		Queue is a valid queue
		POST	All data have been deleted and recycled
		RETURN	NULL pointer
****************************************************************************/
QUEUE *destroyQueue (QUEUE *queue)
{
//Local Definitions
	QUEUE_NODE *dltPtr;

//Statements
	if (queue)  {
		while (queue->front != NULL)  {
			free (queue->front->dataPtr);
			dltPtr = queue->front;
			queue->front = queue->front->next;
			free(dltPtr);
		}//while
		free(queue);
	}
	return NULL;
}//destroyQueue


/****** flushQueue *********************************************************
	Function recycles all nodes from a queue without deleting stored data or
	head node (reset to initial conditions)
		PRE		queue is a pointer to a queue head structure
		POST	all nodes deleted from queue, head structure and stored data
				intact. Count reset to zero.
****************************************************************************/
void flushQueue (QUEUE *queue)
{
//Local Declarations
	QUEUE_NODE *dltPtr;

//Statements
	if (queue)  {
		while (queue->front != NULL)  {
			dltPtr = queue->front;
			queue->front = queue->front->next;
			free(dltPtr);
		}//while
		queue->front = NULL;
		queue->rear = NULL;
		queue->count = 0;
	}
	return;
}//flushQueue

