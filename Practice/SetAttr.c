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
	int policy ;
	pthread_t tid[NO_THREADS] ;
	pthread_attr_t attr ;
	int num, type, state ;

	pthread_attr_init(&attr) ;

	int check = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &type) ;

	if(!check)
	{
		if(type == PTHREAD_CANCEL_ASYNCHRONOUS) printf("ASYNCH\n") ;
		else if(type == PTHREAD_CANCEL_DEFERRED) printf("DIFERR\n") ;
		else printf("ANOTHER THING\n") ;
	}

	check = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &state) ;

	if(!check)
	{
		if(state == PTHREAD_CANCEL_DISABLE) printf("DISABLE\n") ;
		else if(state == PTHREAD_CANCEL_ENABLE) printf("ENABLE\n") ;
		else printf("ANOTHER THING\n") ;
	}

	if(pthread_attr_getschedpolicy(&attr, &policy) != 0)
		fprintf(stderr, "Unable to get policy.\n") ;
	else
	{
		if(policy == SCHED_OTHER) printf("Other\n") ;
		else if(policy == SCHED_RR) printf("RR\n") ;
		else printf("FIFO\n") ;
	}

	if(pthread_attr_setschedpolicy(&attr, SCHED_FIFO) != 0) 
		fprintf(stderr, "Unable to set policy.\n") ;

	if(pthread_attr_getschedpolicy(&attr, &policy) != 0)
		fprintf(stderr, "Unable to get policy.\n") ;
	else
	{
		if(policy == SCHED_OTHER) printf("Other\n") ;
		else if(policy == SCHED_RR) printf("RR\n") ;
		else printf("FIFO\n") ;
	}

	for(num = 0 ; num < NO_THREADS ; num++)
	{
		ret = pthread_create(&tid[num], NULL, func, &num) ;

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
	int *num = (int*)arg ;
	tls = *num ;
	global = *num ;
	// sleep(2) ;
	printf("Thread = %d, tls = %d, global = %d\n", *num, tls, global) ;
}
