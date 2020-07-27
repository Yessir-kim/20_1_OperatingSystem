#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NO_THREADS 3

void *func(void *arg) ;

__thread int tls ; // each thread has this variable
int global ;

int main()
{
	int ret ;
	pthread_t tid[NO_THREADS] ;
	int num ;

	for(num = 0 ; num < NO_THREADS ; num++)
	{
		ret = pthread_create(&tid[num], NULL, func, (void*)num) ;

		if(ret)
		{
			printf("Failed!\n") ;
			exit(1) ;
		}
	}

	for(num = 0 ; num < NO_THREADS ; num++)
	{
		ret = pthread_join(tid[num], NULL) ;

		if(ret)
		{
			printf("Failed!\n") ;
			exit(1) ;
		}
	}

	return 0 ;
}

void *func(void *arg)
{
	int num = (int)arg ;
	tls = num ;
	global = num ;
	sleep(2) ;
	printf("Thread = %d, tls = %d, global = %d\n", num, tls, global) ;
}
