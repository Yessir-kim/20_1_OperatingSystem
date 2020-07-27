#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 512

typedef struct
{
	long mtype ;
	char data[BUFFER_SIZE] ;
} msg_data ;

int repeat_receiver = 1 ; // grobal variable that control receiver loop condition
int ter_col = 0 ;

void *receiver(void* rcv_id) ;
void *sender(void* snd_id) ;

msg_data msg ;

int main(int argc, char *argv[])
{
	int thr_id ; // Check variable that whether success to create
	pthread_t p_thread[2]; // Store thread
	int snd_queue ;
	int rcv_queue ;
	int status ;

 	struct winsize w;

  	ioctl(0, TIOCGWINSZ, &w);

	ter_col = w.ws_col;

	// Not exist two key
	if(argc < 3)
	{
		printf("Usage: %%s <snd_key> <rcv_key>\n") ;
		exit(-1) ;
	}

	// Clear that to input a two key
	int snd_key = atoi(argv[1]) ;
	int rcv_key = atoi(argv[2]) ;

	printf("snd_key = %d, rcv_key = %d\n", snd_key, rcv_key) ;

	/********************************************************
		Creating two msg queue
		If temp_queue == -1,then Failed to create msg queue
	********************************************************/

	if(-1 == (snd_queue = msgget((key_t)snd_key, IPC_CREAT | 0666)))
	{
		perror("Failed to msgget() call") ;
		exit(-1) ;
	}

	if(-1 == (rcv_queue = msgget((key_t)rcv_key, IPC_CREAT | 0666)))
	{
		perror("Failed to msgget() call") ;
		exit(-1) ;
	}

	/********************************************************
		Creating two Thread
		If thr_id == 0, then Failed to create thread
	********************************************************/

	thr_id = pthread_create(&p_thread[0], NULL, sender, &snd_queue) ;
	if(thr_id < 0)
	{
		perror("Thread create error") ;
		exit(-1) ;
	}

	thr_id = pthread_create(&p_thread[1], NULL, receiver, &rcv_queue) ;
	if(thr_id < 0)
	{
	    perror("Thread create error") ;
		exit(-1) ;
	}

	pthread_join(p_thread[0], (void**)&status) ;
	pthread_join(p_thread[1], (void**)&status) ;
	
	msgctl(snd_queue, IPC_RMID, 0) ;
	msgctl(rcv_queue, IPC_RMID, 0) ;

	return 0 ;
}

void *receiver(void *rcv_id)
{
	// msg_data msg ;
	char rcv_msg[BUFFER_SIZE] ;

	msg.data[BUFFER_SIZE] = 0 ; // Clear message buffer
	
	sleep(1) ;

	while(repeat_receiver)
	{
		if(msgrcv(*(int*)rcv_id, &msg, sizeof(msg_data) - sizeof(long), 0,  IPC_NOWAIT) != -1) // Exist message
		{
			strcpy(rcv_msg, msg.data) ;
			
			for(int i = 0 ; i < ter_col/2 ; i++) printf(" ") ;
			
			printf("[incoming] \"%s\"",rcv_msg) ;
			
			rcv_msg[BUFFER_SIZE] = 0 ;
		
			printf("\n[mesg] ") ;

			fflush(stdout) ;
		}
		usleep(1000) ;
	}
}


void *sender(void *snd_id)
{
	msg.mtype = *(int*)snd_id ;

	printf("[mseg] ") ;

	fflush(stdout) ;

	while(1)
	{
		fgets(msg.data, BUFFER_SIZE, stdin) ;
		
		msg.data[strlen(msg.data) - 1] = 0;   // trim '\n'
		
		// strcpy(msg.data, string_buff) ;
	
		if(!strcmp(msg.data, "quit"))
		{
			repeat_receiver = 0 ;
			break ;
		}
		if(-1 == (msgsnd(*(int*)snd_id, &msg, sizeof(msg_data) - sizeof(long), 0)))
		{
			printf("Failed to send message\n") ;
		}

		printf("[mseg] ") ;
	}
}
