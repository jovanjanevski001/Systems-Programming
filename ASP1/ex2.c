#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


int main (int argc, int* argv[])
{
	int bufSize = 1500 * sizeof(char);

	char* str = (char*)malloc(100*sizeof(char));
	char* buffer = (char*)malloc(bufSize);

	stdin = fopen("input.txt", "r");			//	stdin now poiting to input.txt 

	// keep grabbing lines until NULL/EOF
	while (fgets(str, bufSize, stdin) != NULL) 
	{
		// if the string length is 50 characters, then it matches our desired out
		// ---> append the str to the buffer
		if (strlen(str) == 50)
			strcat(buffer, str+9);
	}

	write(STDOUT_FILENO, buffer, bufSize);		//	write buffer to the pipe

	// free up any allocated memory on the heap
	free(str);
	_exit(1);

	return 0;
}
