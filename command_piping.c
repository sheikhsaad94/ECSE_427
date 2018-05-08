/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/

//Please enter your name and McGill ID below
//Name: Sheikh Saad Salah Uddin
//McGill ID: 260569832

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
	{
	//this array contains the command ls to be executed in the child using execvp
	//execvp takes in char* as the command and char* array as the arguments to the command
	//array must be NULL terminated to tell execvp that it is the end of the arguments
	char *arg[3] = {"ls", NULL};
	char readBuf[1024];	

	//create array to represent the pipe
	int pipeEnds[2];

	//create pipe
	//pipeEnds[0] is the read end and pipeEnds[1] is the write end
	pipe(pipeEnds);
	
	//fork process. retVal stores values returned by fork
	int retVal = fork();

	//retVal is negative if fork failed
	if(retVal<0){
		perror("Fork failed\n");
		}

	//Child process when retVal is 0
	else if(retVal == 0){
		//print the value of retVal for verifying that this is inside the child process
		printf("Child %d\n", retVal);

		/*make the write end of the pipe point the same as stdout so that the output
		of this process goes to that end*/
		dup2(pipeEnds[1], STDOUT_FILENO);

		//close the read end of the pipe in the child process
		close(pipeEnds[0]);

		//execute the ls command
		if(execvp(arg[0], arg) == -1){
			//execvp returns -1 if failed
			perror("execvp failed\n");
			}

		//close the write end of the pipe after command finished executing
		close(pipeEnds[1]);		
		}

	//Parent process when retVal is greater than 0
	else if(retVal > 0){
		//print the value of retVal for verifying that this is inside the parent process
		printf("Parent %d\n", retVal);	

		/*make the read end of the pipe point the same as stdin so that the input
		of the child process comes to this end in the parent*/	
		dup2(pipeEnds[0], STDIN_FILENO);

		//close the write end of the pipe in the parent process
		close(pipeEnds[1]);

		//read from the read end of the pipe and output it in the write end
		read(pipeEnds[0], readBuf, sizeof(readBuf));

		//close the read end of the pipe after finished reading
		close(pipeEnds[0]);

		/*print the output of the ls command from the child process in the
		parent process, along the pid to make sure it is being printed in the
		correct process*/
		printf("Process ID: %d %s\n", retVal, readBuf);
		} 
	
	return 0;
	}
