#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 5
#define MAX_MESSAGE 64
char buffer[BUFFER_SIZE][MAX_MESSAGE] ;
int in = 0, out = 0, count = 0 ;

pthread_mutex_t mutex ;
sem_t empty, full ;

int repeat = 1 ;

void DisplayBuffer() ;
void* tProducer(void *arg) ;
void* tConsumer(void *arg) ;

int main(int argc, char* argv[])
{
	int duration, sem_value ;
	pthread_t producer, consumer ;

	if(argc > 1) duration = atoi(argv[1]) ; // defined value by user
	else duration = 30 ; // default value 30

	srand(time(NULL)) ;
	pthread_mutex_init(&mutex, NULL) ;
	sem_init(&empty, 0, BUFFER_SIZE) ;
	sem_init(&full, 0, 0) ;

	DisplayBuffer() ;

	pthread_create(&producer, NULL, tProducer, NULL) ;
	pthread_create(&consumer, NULL, tConsumer, NULL) ;

	sleep(duration) ;
	repeat = 0 ;
	
	sem_getvalue(&full, &sem_value) ;
	if(sem_value == 0) sem_post(&full) ;
	sem_getvalue(&empty, &sem_value) ;
	if(sem_value == 0) sem_post(&empty) ;

	pthread_join(producer, NULL) ;
	pthread_join(consumer, NULL) ;
	
	sem_destroy(&full) ;
	sem_destroy(&empty) ;
	pthread_mutex_destroy(&mutex) ;

	printf("Bye!\n") ;

	return 0 ;
}

void DisplayBuffer()
{
	printf("Buffer contents:\n") ;
	printf("\tcount = %d, in = %d, out = %d\n",count, in, out) ;
	
	int p = out ;
	for(int i = 0 ; i < count ; i++)
	{
		printf("\t%d] %s\n", p, buffer[p]) ;
		p = (p + 1) % BUFFER_SIZE ;
	}
}

void* tProducer(void *arg)
{
	int no_messages = 10 ; 
	char temp[MAX_MESSAGE] ;

	char *messages[64] = {
		"Nice to see you!", 
		"Aal izz well!", 
		"I love you! God loves you!", 
		"God loves you and has a wonderful plan for your life.", 
		"God bless you!", 
		"You are genius!", 
		"Cheer up!", 
		"Everything is gonna be okay.", 
		"You are so precious!", 
		"Hakuna matata!"
	} ;
	
	while(repeat)
	{
		sleep(rand() % 3 + 1) ;
		strcpy(temp, messages[rand() % no_messages]) ;
		printf("[Producer] Created a message \"%s\"\n", temp) ;
		
		sem_wait(&empty) ;
		pthread_mutex_lock(&mutex) ;
		
		printf("---------------------------PRODUCER---------------------------\n") ;
		printf("Producer is entering critical section.\n") ;
		printf("[Producer] \"%s\" ==> buffer\n", temp) ;
		
		strcpy(buffer[in], temp) ;
		in = (in + 1) % BUFFER_SIZE ;
		count++ ;
		
		DisplayBuffer() ;
		printf("Producer is leaving critical section.\n") ;
		printf("--------------------------------------------------------------\n") ;
		
		pthread_mutex_unlock(&mutex) ;
		sem_post(&full) ;
	}
}

void* tConsumer(void *arg)
{
	char temp[MAX_MESSAGE] ;
	while(repeat)
	{
		sem_wait(&full) ;
		pthread_mutex_lock(&mutex) ;
		
		printf("---------------------------CONSUMER---------------------------\n") ;
		printf("Consumer is entering critical section.\n") ;
		printf("[Consumer] \"%s\" ==> buffer\n", buffer[out]) ;
		
		strcpy(temp, buffer[out]) ;
		count-- ;
		out = (out + 1) % BUFFER_SIZE ;
		
		DisplayBuffer() ;
		printf("Consumer is leaving critical section.\n") ;

		pthread_mutex_unlock(&mutex) ;
		sem_post(&empty) ;
		printf("--------------------------------------------------------------\n") ;
		printf("[Consumer] Consumed a message \"%s\"\n", temp) ;
		sleep(rand() % 3 + 2) ;
	}
}	
