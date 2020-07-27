#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <wait.h>

int value = 0 ;
void *runner(void *param) ;

int main()
{
	pid_t pid ;
	pthread_t tid ;
	pthread_attr_t attr ;

	pthread_attr_init(&attr) ;
	pthread_create(&tid, &attr, runner, NULL) ;
	pthread_join(tid, NULL) ;

	pid = fork() ;

	if(pid == 0) // child
	{
		// pthread_attr_init(&attr) ;
		// pthread_create(&tid, &attr, runner, NULL) ;
		// pthread_join(tid, NULL) ;
		printf("CHILD : value = %d\n", value) ;
	}
	else if(pid > 0) // parent
	{
		wait(NULL) ;
		printf("PARENT : value = %d\n", value) ;
	}
	else printf("Failed!") ;
}

void *runner(void *param)
{
	value = 5 ;
	pthread_exit(0) ;
}
		
