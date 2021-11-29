#include "./queue.h"

void* Thread1Function(void*);
void* Thread2Function(void*);

int GetFileAttributes(void);

pthread_mutex_t mutex;
pthread_cond_t cond;

/* Global variables that are shared between threads
 * volatile so the compiler doesnt optimize them */
volatile char flag = TRUE;
volatile __queue__ q;

int main (int argc, char* argv[])
{
	/* User input for the buffer size */
	int bufferSize = atoi(argv[1]);

	/* GetFileAttributes returns - # of lines in the file */
	int lines = GetFileAttributes();
	char* outputBuffer = (char*)malloc(MAX_LINE_LENGTH * lines * sizeof(char));		//	to be passed into t2 by reference
	void* arg = outputBuffer;

	QueueInit(&q, bufferSize);		//	inits q to hold bufferSize items 

	pthread_t t1, t2;

	/* Inits for Mutex and Condition variable */
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	/* Create threads - t2, t1*/ 
	pthread_create(&t2, NULL, Thread2Function, arg);
	pthread_create(&t1, NULL, Thread1Function, NULL);
	
	/* Destroy threads - t2, t1 (return NULL from both) */
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	
	/* Destroy mutex & condition variable */
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);	

	QueueDestroy(q);
	
	// stdout now writes to output.txt, not the terminal
	freopen("output.txt", "w+", stdout);
	write(STDOUT_FILENO, outputBuffer, 100*lines*sizeof(char));

	free(outputBuffer);		//	free allocated memory on the heap...

	return 0;
}

void* Thread1Function(void* arg)
{
	char* temp = (char*)malloc(100*sizeof(char));
	
	// while EOF is not reached, grab a line from stdin buffer
	while (fgets(temp, 100, stdin) != NULL)
	{
		// accessing shared resources, so must lock our mutex to guarantee exclusion
		pthread_mutex_lock(&mutex); 

		// check if the queue is not full
		if (IsFull(&q) == FALSE)
		{
			if (strlen(temp) == 50)
				Enqueue(&q, temp+9);			//	add to the queue if the input is well formed
			pthread_mutex_unlock(&mutex);		
			pthread_cond_signal(&cond);			//	signal the other threads that are waiting on cond to wake/unblock
		}

		// queue is full, sleep and try again later
		pthread_mutex_unlock(&mutex);
		usleep(100);							
	}
	
	/* If here, EOF was reached, set the flag to let t2
	 * know that no more items will be sent to the queue */
	pthread_mutex_lock(&mutex);	
	flag = FALSE;
	pthread_mutex_unlock(&mutex);

	free(temp);
	pthread_cond_signal(&cond);

	return NULL;
}


void* Thread2Function(void* arg)
{
	char* resultString;				//	holds the string that is returned from dequeue
	char* result = (char*)arg;		

	/* Loop until flag is set to FALSE by t1 */
	while (1)
	{
		/* Accessing shared resources, lock the mutex for exclusion */
		pthread_mutex_lock(&mutex);

		// checks if the queue is empty, TRUE ==> have t2 wait for cond to be pulled
		while (IsEmpty(&q) == TRUE && flag == TRUE)
		{
			pthread_cond_wait(&cond, &mutex);			
		}

		// checks if queue is empty and flag is FALSE (no more items to be sent to the queue)
		// if both are vaild ==> then break out of the loop
		if (IsEmpty(&q) == TRUE && flag == FALSE) 
		{
			pthread_mutex_unlock(&mutex);
			break;
		}

		resultString = Dequeue(&q);
		strcat(result, resultString+13);		//	appends resultString to result

		pthread_mutex_unlock(&mutex);
	}
	
	return NULL;
}


/* Input:	None
 * Output:	Line num of input.txt
 */
int GetFileAttributes(void)
{
	int lineNum = 0;
	char* temp = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));

	while ( fgets(temp, MAX_LINE_LENGTH*sizeof(char), stdin) != NULL ) ++lineNum;
	
	rewind(stdin);		//	reset stdin back to the beginning of input.txt
	free(temp);

	return lineNum;
}
