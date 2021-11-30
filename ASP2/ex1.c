#include "./queue.h"

void* Thread1Function(void*);
void* Thread2Function(void*);

int GetFileAttributes(void);

pthread_mutex_t mutex;
pthread_cond_t cond;

/* Global variables that are shared between threads.
 * volatile so the compiler doesnt optimize them. */
volatile char flag = TRUE;
volatile __queue__ q;

int main (int argc, char* argv[])
{
	/* User input for the buffer size */
	int bufferSize = atoi(argv[1]);

	int lines = GetFileAttributes();	// # of lines in the file
	char* outputBuffer = (char*)malloc(MAX_LINE_LENGTH * lines * sizeof(char));		//	to be passed into t2 by reference

	pthread_t t1, t2;

	QueueInit(&q, bufferSize);			//	inits q to hold "bufferSize" amount of items 

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_create(&t2, NULL, Thread2Function, (void*)outputBuffer);
	pthread_create(&t1, NULL, Thread1Function, NULL);
	
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);	

	QueueDestroy(q);
	
	/* stdout now writes to output.txt, not the terminal */
	freopen("output.txt", "w+", stdout);
	write(STDOUT_FILENO, outputBuffer, MAX_LINE_LENGTH * lines * sizeof(char));

	free(outputBuffer);		

	return 0;
}

void* Thread1Function(void* arg)
{
	char* temp = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
	
	// while EOF is not reached, grab a line from stdin buffer
	while (fgets(temp, MAX_LINE_LENGTH, stdin) != NULL)
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

		// have t2 wait for cond to be pulled if the queue is empty
		while (IsEmpty(&q) == TRUE && flag == TRUE)
			pthread_cond_wait(&cond, &mutex);			

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
	
	rewind(stdin);		//	set stdin to the beginning of input.txt
	free(temp);

	return lineNum;
}
