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
#include <unistd.h>
#include <fcntl.h>

int main()
	{
	//save stdout for restoring later
	int saved_stdout = dup(1);	

	//print to stdout	
	printf("First:Print to stdout\n");

	//open file. Create file if it does not already exist.
	int fd = open("redirect_out.txt", O_WRONLY | O_CREAT | O_APPEND, 0777);
	
	//print if error opening file
	if(fd<0)
		perror("Error opening the file\n");	
	
	//close stdout
	close(1);	

	//replace stdout with the file on which the output will be printed
	int copyd = dup(fd);
	close(fd);
	
	//print to file
	printf("Second:Print to redirect_out.txt\n");
	
	//restore stdout
	dup2(saved_stdout,1);
	
	//print to stdout
	printf("Third:Print to stdout\n");
	return 0;
	}
