// If the comments are hard to read because of the color,
// type ':set background=dark'

/***

	This program reads texts and switches the case of the characters (lower case <-> upper case) until the user types "quit".

	Example)
		Input a text: Welcome!
		convered text = wELCOME!
		Input a text: God bless you!
		convered text = gOD BLESS YOU!
		Input a text: quit
		Bye!

	It launches a child process and communicates with it through two ordinary pipes, one to send the original text and the other to receive the converted text.

	Complete the program using ORDINARY PIPES by following the instructions.
	DO NOT use other IPC such as shared memory or message queue.

	The main creates two pipes and a child process.
	
	Then, the parent process repeats the followings until the user types "quit".
		Read a text line from the user.
		Send the text to the child through pipe.
		Receive and display the converted text.

	The child process repeats the followings until it receives "quit" from the parent. 
		Read a text line from the parent through one pipe.
		Convert all upper case characters to the corresponding lower case characters and vice versa.
		Write the converted text to the parent through the other pipe.

	Hint) To read a text line from the user, use the following code.
			printf("Input a text: ");
			fgets(src, BUFFER_SIZE - 1, stdin);	
			int len = strlen(src) - 1;
			src[len] = 0;				// trim '\n'

	Note! Before the parent terminates, there the child processing MUST terminate.
		You can check whether the child is still running by check currently running processes by 'ps -al'.

***/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

#define READ_END 0
#define WRITE_END 1

#define BUFFER_SIZE 256

void SwitchCase(int in[], int out[]);
int repeat = 1 ;
int main()
{
	int in[2];		// pipe from parent to child
	int out[2];		// pipe from child to parent
	pid_t pid ;
	char src[BUFFER_SIZE] ;

	//	TO DO: Create two ordinary pipes
	if(pipe(in) == -1 || pipe(out) == -1)
	{
		fprintf(stderr, "Pipe failed") ;
		return 1 ;
	}

	//  TO DO: Create child process
	pid = fork() ;

	if(pid < 0)
	{
			fprintf(stderr, "Fork failed") ;
			return 1 ;
	}
	// On parent process,
	if(pid > 0)
	{
	// Then, the parent process repeats the followings until the user types "quit".
		while(repeat)
		{
			// Read a text line from the user.
			printf("Input a text: ");
			fgets(src, BUFFER_SIZE - 1, stdin);	
			int len = strlen(src) - 1;
			src[len] = 0;				// trim '\n'

			// Send the text to the child through pipe.
			write(in[WRITE_END], src, strlen(src) + 1) ;
			
			if(strcmp(src,"quit") == 0) break ;
			// Receive and display the converted text.
			read(out[READ_END], src, BUFFER_SIZE) ;
			printf("Converted text : %s\n", src) ;
		}
		wait(NULL) ;
		printf("good bye!\n") ;
	}
	else
	{
		SwitchCase(in, out) ;
	}

	close(in[READ_END]) ;
	close(in[WRITE_END]) ;

	return 0;
}

void SwitchCase(int in[], int out[])
{
	char buffer[BUFFER_SIZE] ;
	char temp[BUFFER_SIZE] ;
	// TO DO: Implement the following algorithm
	// Repeats the followings until it receives "quit" from the parent. 
	while(repeat)
	{
		//	Receive a text line from the parent through pipe in.
		read(in[READ_END], buffer, BUFFER_SIZE) ;
		
		if(strcmp(buffer, "quit") == 0) break ;

		//	Convert all upper case characters to the corresponding lower case characters and vice versa.
		for(int i = 0 ; i < strlen(buffer) ; i++)
		{
				if(isupper(buffer[i])) buffer[i] = tolower(buffer[i]) ;
				else buffer[i] = toupper(buffer[i]) ;
		}
		//	Send the converted text to the parent through pipe out.
		write(out[WRITE_END], buffer, strlen(buffer) + 1) ;
		sleep(1) ;
	}
	// TO DO: deallocate pipes;
	close(out[READ_END]) ;
	close(out[WRITE_END]) ;
}

