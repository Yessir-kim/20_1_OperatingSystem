// If the comments are hard to read because of the color,
// type ':set background=dark'

/***

	This program moves bars left and right until the user types 'q' key.
	Usage: $ ./bar <no_bar>

	Currently, it moves only one bar.
	Extend this program to move multiple bars using multiple threads.
	- Launch multiple threads, each of which moves a bar.
	- The vertical interval between bars should be height / (no_bar + 1).

	Compile with Console.c with -pthread -D_LINUX flag
		Ex) gcc bar.c Console.c -pthread -D_LINUX


***/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "Console.h"

#define TRUE 1
#define FALSE 0

typedef struct{
		int x ;
		int y ;
		char len ;
		int width ;
		int dx ;
		int idx ;
} ThreadParam ;

int repeat = TRUE;

void DrawBar(int x, int y, int len, char c);
void* BarThread(void *vparam) ;

int main(int argc, char *argv[])
{
	int no_bar = 0;

	if(argc > 1)
		no_bar =atoi(argv[1]);
	if(no_bar == 0)
		no_bar = 5;

	srand(time(NULL));
	int width = getWindowWidth();
	int height = getWindowHeight() - 1;
	pthread_t tid[no_bar] ;
	ThreadParam param[no_bar] ;
	
	clrscr();
	gotoxy(1, 1);
 	printf("Press 'q' to quit.\n");		
	
	for(int i = 0 ; i < no_bar ; i++)
	{
		param[i].width = width ;
		param[i].idx = i ;
		param[i].len = rand() % 6 + 5;	// a random number in [5, 10]
		int step = rand() % 4 + 2;	// a random number in [2, 5]
		param[i].x = rand() % (width - step - 1) + 1; 

		// vertical interval between bars = height / (no_bar + 1)
		float interval = height / (float)(no_bar + 1) * (i + 1);
		param[i].y = (int) interval;

		param[i].dx = step;
		if(rand() % 2 == 0) param[i].dx = -param[i].dx;
		//printf("%d Thread is created!\n",i+1) ;
		pthread_create(&tid[i], NULL, BarThread, &param[i]) ;
	}

//  Do not enable the next line until you complete the program.
//	EnableCursor(FALSE);

	// TO DO: Wait for 'q' key. (use getch() and while statement)
	while(repeat) if(getch() == 'q') repeat = FALSE ;
	// TO DO: Stop threads by setting repeat to FALSE

	// TO DO: Wait for the child threads
	for(int i = 0 ; i < no_bar ; i++) pthread_join(tid[i], NULL) ;
	gotoxy(1, 1);
	printf("Bye!\n");

//  Do not enable the next line until you complete the program.
//	EnableCursor(TRUE);

	return 0;
}

void DrawBar(int x, int y, int len, char c)
{
	pthread_mutex_lock(&display_mutex);

	gotoxy(x, y);
	for(int i = 0; i < len; i++)
		putchar(c);
	fflush(stdout);

	pthread_mutex_unlock(&display_mutex);
}

void* BarThread(void *vparam)
{
	ThreadParam *param = (ThreadParam*)vparam ;

	char c = 'A' + param->idx;
	DrawBar(param->x, param->y, param->len, 1);
	while(repeat){
		int oldx = param->x;

		param->x += param->dx;
		if(param->x <= 0){
			param->dx = -param->dx;
			param->x = 1 - param->x;
		}

		if(param->x > param->width - param->len + 1){
			param->dx = -param->dx;
			param->x = param->width - param->len + 1 - (param->x - param->width + param->len - 1);
		}
	
		DrawBar(oldx, param->y, param->len, ' ');
		DrawBar(param->x, param->y, param->len, c);

		MySleep(50);
	}
}
