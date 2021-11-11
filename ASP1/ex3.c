#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main (int argc, int* argv[])
{
	int bufferSize = 1500 * sizeof(char);
	int index = 13;
	int i = 0;	

	char* str = (char*)malloc(bufferSize);
	char* buffer = (char*)malloc(bufferSize);

	read(STDIN_FILENO, buffer, bufferSize);			//	read from the pipe into buffer

	// loop until terminating char is found
	// populate our str with the manipulated result of buffer
	while(buffer[index] != '\0')
	{
		str[i] = buffer[index];
		i++;

		if (buffer[index] == '\n')
			index += 14;

		else
			index++;
	}

	write(STDOUT_FILENO, str, bufferSize);			//	write str to the pipe

	// free any allocated memory from the heap
	free(str);
	free(buffer);
	_exit(1);

	return 0;
}
