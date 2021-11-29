Background Information:
-------------------------

This program utilizes the pthread library and creates 2 threads (t1, t2) that will run concurrently.


- t1 is tasked with reading input.txt and manipulating the strings. After manipulating the current line,
	 it will add the item to the buffer.

- t2 is tasked with removing the items from the buffer and performing further manipulations to the strings.
	It will be writing these resulting strings into a buffer that was passed by reference to t2.



-------------			---------			---------			--------			--------------
| Input.txt |	 ==>		|  t1	|	|======>	|  t2	|	 ==>		| MAIN |	 ==>		| Output.txt |
-------------   STDIN		---------	 BUFFER		---------	RETURNS		--------	STDOUT		--------------


STDIN	-	points to input.txt and reads into t1

BUFFER	-	A bounded circular buffer using a queue implementation. 
			The size of the buffer is determined by user input.
			t1 will add items to the buffer while t2 removes them.
			
			Items can only be added if the buffer is not full
			Items can only be removed if the buffer is not empty

RETURNS	-	Returns back to main after t1 and t2 have exited. 

STDOUT	-	writes to output.txt as opposed to the console


Compilation Instructions:
-----------------------------------

$ make
$ ./locationUpdater 10 < input.txt
$ cat output.txt

where 10 is the size of the buffer

-----------------------------------


To remove all generated output executables and output text files:
-----------------------------------------------------------------

$ make clean
