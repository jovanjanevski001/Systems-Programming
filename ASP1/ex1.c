#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define RD	0
#define WR	1

int main (int argc, int* argv[])
{
	int fds[2];		
	pid_t pid, pid2;
	
	char* list[2] = {"./ex2", NULL};
	char* args[2] = {"./ex3", NULL};

	
	/* opening pipe + error check */ 
	if (pipe(fds) == -1)
		printf("Error opening pipe...\n");
	

	pid = fork();		//	create child1 process

	switch (pid)
	{
		// Cannot fork...
		case -1:
			printf("error during fork");
			break;

		// Child 1, responsible for running email_filter	
		case 0:
			/* close unused pipes */
			close(fds[RD]);
			
			dup2(fds[WR], STDOUT_FILENO);		//	write end of pipe duplicated to stdout fileno
			close(fds[WR]);						//	dont need this end of the pipe either after dup2 call
			
			execv(list[0], list);				//	load the process with a new program executable and arg list (if any)

		// Parent code			
		default:	
		
		   	pid2 = fork();				//	create the 2nd child process

			// Child 2, responsible for running calender_filter
			if (pid2 == 0)
			{
				dup2(fds[RD], STDIN_FILENO);	//	read end of pipe duplicated to stdin fileno
				close(fds[RD]);					//	pipe not needed after dup2 call

				dup2(fds[WR], STDOUT_FILENO);	//	write end of pipe duplicated to stdout fileno
				close(fds[WR]);					//	pipe not needed afer dup2 call

				execv(args[0], args);			//	run the ./ex3 executable
			}

			else
			{
				/*parent process */
				int status = 0;
				int bufSize = 1500* sizeof(char);
				char* str = (char*)malloc(bufSize);

				pid_t wpid;

				// close unused pipes
				close(fds[WR]);									
			
				// wait while the children are not finished
				while ( (wpid = wait(&status)) > 0 )

				// read from the pipe
				read(fds[RD], str, bufSize);

				freopen("output.txt", "w+", stdout);					// redirect stdout to output.txt
				printf("%s", str);										// printing now writes to our output text file, not the console
				
				/* free any memory allocated on the heap and close all the remaining open pipes below */
				free(str);
				close(fds[RD]);

				exit(1);
			}
	}
	return 0;
}
