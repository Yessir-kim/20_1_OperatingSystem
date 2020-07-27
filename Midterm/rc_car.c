// If the comments are hard to read because of the color,
// type ':set background=dark'

/***

	This program moves a car ('*' mark) using two processes.

	The parent reads key strokes from the user and send it to the child.

	The child moves the car according to the key received from the parent.
		Move: the arrow keys (left, up, right, down)
		Stop: space bar
		Quit: 'q'

	Complete this program using Linux message queues API.
 		(msgget(), msgsnd(), msgrcv(), and msgctl())

	In the main()
		Create a message queue msgq.
			On failure, print an error message.
		Create a child process.

		On the parent proocess,
			Call Controller(msgq).
			Wait for the child to terminate.
			Deallocate msgq.

		On the child process,
			Call MoveCar(msgq).

	The parent process runs Controller().
		Repeat the followings until the user types 'q' key.
			Read a key from the user using getch();
			Send the key to the child process through message queue.
				On failure, print an error message.

	The child process runs MoveCar().
		Move the car according to the key received from the parent through mesgq.
		Complete the MoveCar() function following the instructions marked as 'TO DO'

	Compile with Console.c with -pthread -D_LINUX flag
		Ex) gcc rc_car.c Console.c -pthread -D_LINUX

***/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

#include "Console.h"
#define TRUE 1
#define FALSE 0

typedef struct {		// Use this structure to send messages.
	long data_type;
	int key; 
} Message;

Message msg ;

void Controller(int msgq);
void MoveCar(int msgq);

int main()
{
	key_t key ;
	int msgq = 0;	// id of message queue
	msgq = msgget(key, IPC_CREAT | 0666) ; // TO DO: Create a message queue (msgq)
	pid_t pid ;
	
	if(msgq == -1)
	{
		perror("Failed to msgget() call") ;
		exit(-1) ;
	}
	// 		On failure, print an error message and exit.

	clrscr();
	gotoxy(1, 1);
	printf("Move the car with the arrow keys, space(stop), or q(quit).\n");

	// TO DO: Implement the following alogrithm.

	// Create a child process
	pid = fork() ;
	// On the parent process
	if(pid > 0)
	{
		Controller(msgq) ;
		wait(NULL) ;// Wait for the child process
		msgctl(msgq, IPC_RMID, 0) ;// Remove the message queue
	}
	else if(pid == 0)// On the child process
	{
		MoveCar(msgq) ;
	}
	else perror("Failed to fork") ;

	return 0;
}

void Controller(int msgq)
{
	//Message msg ;
	msg.data_type = 1 ;

	while(1)
	{
		int c = getch() ;
		msg.key = c ;

		if(-1 == (msgsnd(msgq, &msg, sizeof(Message) - sizeof(long), 0)))
			printf("Failed to send message\n") ;
	
		if(c == 'q') break ;
	}
	// Repeat until the user types 'q'
		// Read a key using getch()
		// Send the key through msgq 
		//		On failure, print an error message.
}

void MoveCar(int msgq)
{
	//msg.data_type = msgq ;
	int width = getWindowWidth();
	int height = getWindowHeight() - 1;

	int x = width / 2;
	int y = height / 2;
	int dx = 0;
	int dy = 0;
	Message msg ;
//  Do not enable the next line until you complete the program.
	EnableCursor(FALSE);

	// initially, draw car at the center
	gotoxy(x, y);
	putchar('*');
	fflush(stdout);

	int repeat = 1;
	while(repeat){
		int ret = 0;
		int key = 0;

		ret = msgrcv(msgq, &msg, sizeof(Message) - sizeof(long), 0, IPC_NOWAIT) ;
		key = msg.key ;
		//printf("%d",ret) ;
		// TO DO: Receive the key from msgq. (use IPC_NOWAIT flag)
		// 		DO NOT call kbhit() or getch() on the child process
			
		if(ret > 0){		// if a key was received
			gotoxy(1, height);
			printf("ret = %d, key = %d (%c) ", ret, key, key); 
			fflush(stdout);

			// set direction according to the key
			switch(key){
				case ' ':			// space
					dx = dy = 0;
					break;

				case 68:			// left
					dx = -1;
					dy = 0;
					break;

				case 65:			// up
					dx = 0;
					dy = -1;
					break;

				case 67:			// right
					dx = 1;
					dy = 0;
					break;

				case 66:			// down
					dx = 0;
					dy = 1;
					break;
	
				case 'q':
					repeat = 0;
					break;
			}
		}
		// erase previous coordinate
		//printf("%d, %d", x, y) ;
		gotoxy(x, y);
		putchar(' ');

		// update coordinate
		x += dx;
		if(x < 1){
			x = 1;
			dx = 0;
			//printf("!!!!!!!!!!!!!!!!!!!!!") ;
		} else if(x > width){
			x = width;
			dx = 0;
		}	

		y += dy;
		if(y < 1){
			y = 1;
			dy = 0;
			//printf("@@@@@@@@@@@@@@@@@@@") ;
		} else if(y > height){
			y = height;
			dy = 0;
		}

		// draw star at the new coordinate
		gotoxy(x, y);
		//printf("%d, %d", x, y) ;
		putchar('*');

		gotoxy(width - 2, 1);
	
		fflush(stdout);
		MySleep(50);
	}

//  Do not enable the next line until you complete the program.
	EnableCursor(TRUE);

	clrscr();
	gotoxy(1, 1);
	printf("Bye!\n");
}

