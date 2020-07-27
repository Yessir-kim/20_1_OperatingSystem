#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>

typedef struct{
		char message[512];
		int flag ;
} shm_meaage ;

void parent(int shm_id);		// function for the parent
void child(int shm_id);			// function for the child

int main(int argc, char argv[0])
{
	int shm_id = 0;
	const char *name = "shm_os" ;

	shm_id = shm_open(name, O_CREAT | O_RDWR, 0666) ;
	ftruncate(shm_id, sizeof(shm_meaage)) ;

	pid_t child_pid = fork();

	if(child_pid < 0){
		printf("Failed to create child process\n");
		exit(-1);
	} else if(child_pid > 0){
		// parent
		parent(shm_id);
		shm_unlink(name) ; // At parent or child, only one excute 
	} else {
		// child
		child(shm_id);
	}

	sleep(1) ;

	return 0;
}

void parent(int shm_id)
{
	void *temp ;
	temp = mmap(0, sizeof(shm_meaage), PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0) ;
	shm_meaage *buffer = (shm_meaage*)temp ;

	sleep(2);

	printf("[parent] Input a message : ") ;

	char message[512] ;
	message[511] = 0 ;

	fgets(message, 512, stdin);
	message[strlen(message) - 1] = 0;

	strcpy(buffer->message,message) ;

	buffer->flag = 1 ;

	munmap(buffer, sizeof(shm_meaage)) ;
}

void child(int shm_id)
{
	sleep(1);

	printf("[child] Started\n");
	fflush(stdout);

	void *temp ;
	temp = mmap(0, sizeof(shm_meaage), PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0) ;
	shm_meaage *buffer = (shm_meaage*)temp ;

	while(!buffer->flag) ;

	printf("[child] %s\n", (char*)buffer);
	fflush(stdout) ;

	munmap(buffer, sizeof(shm_meaage)) ;
}
