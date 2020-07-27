#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

#include "Console.h"

// use this structure to send parameter to the child threads
typedef struct {
	int width, height;		// screen size
	int x, y;				// current coordinate of the ball
	int dx, dy;				// velocity of the ball
} ThreadParam;

// initialize the parameter for the thread function
void InitParam(ThreadParam *param, int width, int height);

// thread function to move a ball
void* MoveBall(void *vparam);

int repeat = TRUE;

int main(int argc, char *argv[])
{
	// get <# of balls> from command line parameter
	int no_thread = 0 ;
	if(argc > 1)
		no_thread = atoi(argv[1]) ;
	if(no_thread == 0)				// for safety
		no_thread = 5 ;				// default value of <# of threads>

	srand(time(NULL)) ;

	int width = getWindowWidth() ;
	int height = getWindowHeight() - 1 ;
	int status ;

	EnableCursor(0) ;				// hide cursor

	clrscr() ;
	printf("screen size = %d x %d\n", width, height) ;
	printf("Press ESC to quit!\n") ;

	ThreadParam param[no_thread] ;

	pthread_t thread_t[no_thread] ;

	for(int i = 0 ; i < no_thread ; i++)
	{
		InitParam(&param[i], width, height) ;
		pthread_create(&thread_t[i], NULL, MoveBall, (void*)&param[i]) ;
	}

	while(getch() != 27) MySleep(1000) ;

	repeat = FALSE ; // when user input a ESC

	for(int i = 0 ; i < no_thread ; i++) pthread_join(thread_t[i], (void**)&status) ;

	clrscr() ;
	gotoxy(1, 1) ;
	printf("Bye!\n") ;

 	EnableCursor(1) ;			// enable cursor

	return 0 ;
}

void InitParam(ThreadParam *param, int width, int height)
{

	param->width = width ;
	param->height = height ;
	param->x = rand() % width + 1 ;
	param->y = rand() % height + 1 ;
	param->dx = rand() % 7 - 3 ;        // dx in [-3, +3]
	param->dy = rand() % 5 - 2 ;        // dy in [-2, +2]
}

void* MoveBall(void *vparam)
{
	ThreadParam *param = (ThreadParam*) vparam ;

	while(repeat){
		// save current coordinate
		int oldx = param->x ;
		int oldy = param->y ;

		// update horizontal coordinate
		param->x += param->dx ;

		// horizontal bouncing
		if(param->x <= 0){
			param->x = 1 + (1 - param->x) ;
			param->dx = -param->dx ;
		}
		else if(param->x > param->width){
			param->x = param->width - (param->x - param->width) - 1 ;
			param->dx = -param->dx ;
		}

		// update vertical coordinate
		param->y += param->dy ;

		// vertical bouncing
		if(param->y <= 0){
			param->y = 1 + (1 - param->y) ;
			param->dy = -param->dy ;
		}
		else if(param->y > param->height){
			param->y = param->height - (param->y - param->height) - 1 ;
			param->dy = -param->dy ;
		}

		// delete previous ball
		PrintXY(oldx, oldy, " ") ;

		// draw new ball
		PrintXY(param->x, param->y, "*") ;

		// delay
		MySleep(50) ;
	}

	return NULL ;
}
