Background information:
----------------------------------
This exercise demonstrates the use of pipe(), fork(), execv(), dup/dup2()
to run multiple processes and chain the output of one process as input to another.


--------------		------------	Stage1	   -----------	   Stage2     ----------               --------------
|  Input.txt |   ==>    |  Child 1 |   0======0    | Child 2 |    0======0    | Parent |     ==>       | Output.txt |
--------------  STDIN   ------------	 PIPE      -----------	    PIPE      ----------    STDOUT     --------------

STDIN 	- points to input.txt and reads into child 1

Stage1	- Child 1 uses the write end of PIPE to send data to Child 2. 
	    * Child 2 will use the read end of PIPE to read the data sent from Child 1.

Stage2	- Child 2 uses the write end of PIPE 2 to send data to the Parent process.
	    * Parent uses the read end of PIPE 2 to read the data sent from Child 2.
	    
STDOUT	- writes to output.txt as opposed to the console


Child 1 process will be running ./ex2 using execv
Child 2 process will be running ./ex3 using execv

Compiliation instructions:
----------------------------------

$ make
$ ./ex1
$ cat output.txt

----------------------------------

To remove all generated output executables and output txt files:
------------------------------------------------------------------------

$ make clean
