#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE    1
#define FALSE   0
#define MAX_LINE_LENGTH	100


/* Implementing a circular queue using an array */
typedef struct __queue__
{
    char** str_array;
    int head, tail, num_entries, size;
    
}__queue__;


/* Function to initialize the queue */
void QueueInit(volatile __queue__* q, int size)
{
	/* str_array will be an array of strings of size MAX_LINE_LENGTH * size
	 * size will be determined by user input */
	q->str_array = (char**)malloc(MAX_LINE_LENGTH * size * sizeof(char));
    q->head = 0;
    q->tail = 0;
    q->num_entries = 0;
    q->size = size;
}

/* Since we allocated memory on the heap, proper practice to free it when done */
void QueueDestroy(volatile __queue__ q)
{
    free(q.str_array);
    return;
}

/* Function to check if queue is empty */
int IsEmpty(volatile __queue__* q)
{
    return ( (q->num_entries == 0) ? TRUE : FALSE );    
}

/* Function to check if queue is full */
int IsFull(volatile __queue__ * q)
{
    return ( (q->num_entries == q->size) ? TRUE : FALSE );
}

/* Add str to the queue */ 
void Enqueue(volatile __queue__* q, char* str)
{
    // check if queue is full
    if (IsFull(q) == TRUE)
        return;
    
    // add the data to the tail of the queue
    q->str_array[q->tail] = str;
    q->num_entries++;
    
    // to handle the position of the tail, modulus to wrap back around, avoids an if check...
    q->tail = (q->tail + 1) % (q->size);
	
    return;
}

/* Remove the oldest item of the queue */
char* Dequeue(volatile __queue__* q)
{
    char* retStr;
   
	// checks if the queue is empty	
    if (IsEmpty(q) == TRUE)
        return NULL;
        
	// grabs the item at head index
    retStr = q->str_array[q->head];
    q->num_entries--;

	// adjust the head index now, using modulus to handle wraping
    q->head = (q->head + 1) % (q->size);

    return retStr;
}

