#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END	0
#define WRITE_END	1

#define CHILD1	0
#define CHILD2	1

int main (int argc, int* argv[])
{

//	 fds[0][0] --> child 1's read end
//	 fds[0][1] --> child 1's write end

//	 fds[1][0] --> child 2's read end
//	 fds[1][1] --> child 2's write end	
	int fds[2][2];		
	pid_t pid, pid2;
	
	char* list[2] = {"./ex2", NULL};
	char* args[2] = {"./ex3", NULL};

	// Open the pipes, output which pipe failed to open in case of error
	for (int i = 0; i < 2; i++)
	{
		if (pipe(fds[i]) == -1)
			printf("Error opening pipe #%d\n", i);
	}

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
			close(fds[CHILD2][READ_END]);
			close(fds[CHILD2][WRITE_END]);
			close(fds[CHILD1][READ_END]);			
			
			dup2(fds[CHILD1][WRITE_END], STDOUT_FILENO);		//	write end of pipe redirect to stdout fileno
			close(fds[CHILD1][WRITE_END]);						//	dont need this end of the pipe either after dup2 call
			
			execv("./ex2", list);		//	load the process with a new program executable and arg list (if any)
			printf("Error with execv if this prints!!!");		// if execv works properly, the process is loaded with a new program and any code under the execv call will NOT run

		// Parent code			
		default:	

			pid2 = fork();			//	create the 2nd child process

			// Child 2, responsible for running calender_filter
			if (pid2 == 0)
			{
				/* close unused pipes */
				close(fds[CHILD1][WRITE_END]);
				close(fds[CHILD2][READ_END]);

				dup2(fds[CHILD1][READ_END], STDIN_FILENO);		//	read end of pipe directed to stdin fileno
				close(fds[CHILD1][READ_END]);					//	pipe not needed after dup2 call

				dup2(fds[CHILD2][WRITE_END], STDOUT_FILENO);
				close(fds[CHILD2][WRITE_END]);

				execv("./ex3", args);							//	run the ./ex3 executable
			}

			else
			{
				/*parent process */
				int bufSize = 1500* sizeof(char);
				char* str = (char*)malloc(bufSize);

				// close unused pipes
				close(fds[CHILD1][READ_END]);
				close(fds[CHILD1][WRITE_END]);
				close(fds[CHILD2][WRITE_END]);									
			
				// while the children are not finished
				while ((pid = wait(NULL)) != -1 && (pid2 = wait(NULL) != -1))
		
				// read from the child2 pipe
				read(fds[CHILD2][0], str, bufSize);

				freopen("output.txt", "w+", stdout);					// redirect stdout to output.txt
				printf("%s", str);										// printing now writes to our output text file, not the console

				free(str);
				close(fds[CHILD2][0]);

				exit(1);
			}
	}
	return 0;
}
