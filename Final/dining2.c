char notice[] = "If the following comments are too dark to read, type ':set background=dark'.";

/*****
	ECE30021 Operating Systems (2020 Spring) Final Test

	아래 프로그램은 Dining Philosophers Problem에 대한 해법이다.
	Linux환경에서 vi, gcc, pthread를 사용하여 완성하시요.

	Philosopher의 규칙은 다음과 같다.

	- 5 philosophers sitting on a circular table (thinking or eating)
	- 5 bowls, 5 single chopsticks
	- No interaction with colleagues
	- To eat, the philosopher should pick up two chopsticks closest to her
	- A philosopher can pick up only one chopstick at a time
	- When she finish eating, she release chopsticks

	데모 실행파일 dining 실행 방법:

        chmod u+x dining     // set execution permission
        ./dining 20 	     // run for 20 sec

    main()은 thread function Philosopher()를 이용해 5개의 philosopher threads를 생성한다.

    교재에 소개된 The Dining-Philosopher Problem 해법 알고리즘(using monitor)을 참조하시오.

    Problem 1: pthread_mutex_t와 pthread_cond_t 이용해서 아래 프로그램을 완성하시오.
        - 다음 API 함수들을 사용하시오.
		int pthread_mutex_init(pthread_mutex_t * mutex, const pthread_mutex_attr *attr);
		int pthread_mutex_lock(pthread_mutex_t *mutex);
		int pthread_mutex_unlock(pthread_mutex_t *mutex);
		int pthread_mutex_destory(pthread_mutex_t *mutex);

		int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);
		int pthread_cond_wait( pthread_cond_t* cond, pthread_mutex_t* mutex);
		int pthread_cond_signal(pthread_cond_t *cond);
		int pthread_cond_destroy(pthread_cond_t *cond);

        - 완성된 코드는  dining1.c 라는 이름의 파일로 제출하시오.

    Problem 2: pthread_cond_t 및 관련 함수들을 이용하지 말고 pthread_mutex_t, sem_t, int를 이용하여 완성하시오.
		- 강의자료 중 'Implementing a Monitor Using Semaphores' 부분을 참조하시오.
        - 다음 API 함수들을 사용하시오.
		int sem_init(sem_t *sem, int pshared, unsigned int value);
		int sem_wait(sem_t *sem);
		int sem_post(sem_t *sem);
		int sem_destroy(sem_t *sem);

		- 완성된 코드는  dining2.c 라는 이름의 파일로 제출하시오.


    필요할 경우 structure에field를 추가하는 것은 허용되나 global variable은 추가하지 마시오.
    두 문제의 답안은 HISNet 과제 보드에 각각 제출하시오.
    두 파일은 모두 dining처럼 동작해야 함.

*****/


#include <stdio.h>
#include <stdlib.h>

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

enum STATUS { THINKING, HUNGRY, EATING };

typedef struct {
	int state[5];

	pthread_mutex_t mutex ;
	sem_t next ;// to ensure mutual exclusion of functions
	sem_t var ;
	int v_count ;
	int next_count ;
	// TO DO: declare variables for synchronization (e.g., condition vars, semaphores, int vars)

} DiningPhilosophers;

// The following functions correspond to the methods of Monitor
void InitDiningPhilosophers(DiningPhilosophers *dp);
void DestroyDiningPhilosophers(DiningPhilosophers *dp);
void PickUp(DiningPhilosophers *dp, int i);		// This function MUST be mutually exclusive.
void PutDown(DiningPhilosophers *dp, int i);	// This function MUST be mutually exclusive.
void Test(DiningPhilosophers *dp, int i);

int CheckPhilosophers(DiningPhilosophers *dp);	// check violation

void msleep(int msec);			// wait for msec miliseconds

typedef struct {
	int id;
	DiningPhilosophers *dp;
} ThreadParam;

void* Philosopher(void *vparam);	// thread function

int repeat = TRUE;

int main(int argc, char *argv[])
{
	if(argc == 1){
		printf("Usage: %s <duration>\n", argv[0]);
		return 0;
	}
	int sem ;
	int duration = atoi(argv[1]);
	if(duration == 0)
		duration = 20;

	printf("duration = %d\n", duration);

	srand(time(NULL));

	DiningPhilosophers dp;
	InitDiningPhilosophers(&dp);

	pthread_t tid[5] ;
	ThreadParam param[5] ;
	// TO DO: launch 5 threads using Philosopher()
	for(int i = 0 ; i < 5 ; i++)
	{
		param[i].dp = &dp ;
		param[i].id = i ;
		pthread_create(&tid[i], NULL, Philosopher, &param[i]) ;
	}
	sleep(duration);		// wait for duration seconds

	
	repeat = FALSE;
	
	sem_getvalue(&dp.next, &sem) ;
	if(sem == 0) sem_post(&dp.next) ;
	sem_getvalue(&dp.var, &sem) ;
	if(sem == 0) sem_post(&dp.var) ;

	for(int i = 0 ; i < 5 ; i++) pthread_join(tid[i], NULL) ;
	// TO DO: wait for the threads

	DestroyDiningPhilosophers(&dp);

	printf("Bye!\n");

	return 0;
}

void* Philosopher(void *vparam)
// Thread function for each philosopher
// DO NOT modify this function
{
	ThreadParam *param = (ThreadParam*) vparam;

	while(repeat){
		printf("[Philosopher %d] Thinking...\n", param->id);
		fflush(stdout);

		msleep(rand() % 2000 + 1000);

		PickUp(param->dp, param->id);

		printf("[Philosopher %d] Eating...\n", param->id);
		fflush(stdout);

		CheckPhilosophers(param->dp);

		msleep(rand() % 2000 + 1000);

		PutDown(param->dp, param->id);
	}
}

void msleep(int msec)
{
	if(repeat)
		usleep(msec * 1000);
}


void InitDiningPhilosophers(DiningPhilosophers *dp)
// TO DO: implement this function
{
	dp->v_count = 0 ;
	dp->next_count = 0 ;
	for(int i = 0 ; i < 5 ; i++) dp->state[i] = THINKING ;
	// TO DO: initialize fields of dp and create synchronization objects
	pthread_mutex_init(&dp->mutex, NULL) ;
	sem_init(&dp->next, 0, 0) ;
	sem_init(&dp->var, 0, 0) ;
}

void DestroyDiningPhilosophers(DiningPhilosophers *dp)
// TO DO: implement this function
{
	// TO DO: deallocate all objects in dp
	pthread_mutex_destroy(&dp->mutex) ;
	sem_destroy(&dp->var) ;
    sem_destroy(&dp->next) ;
}

void PickUp(DiningPhilosophers *dp, int i)
// TO DO: implement this function similar to the methods of the monitor
// 	      i.e., running this function from multiple threads simultaneously MUST be prevented.
{
	dp->state[i] = HUNGRY ;
	Test(dp, i) ;
	//pthread_mutex_lock(&dp->mutex) ;
	if(dp->state[i] != EATING)
	{
		dp->v_count++ ;
		if(dp->next_count > 0) sem_post(&dp->next) ;
		else pthread_mutex_unlock(&dp->mutex) ;
		sem_wait(&dp->var) ;
		dp->v_count-- ;
	}
}

void PutDown(DiningPhilosophers *dp, int i)
// TO DO: implement this function similar to the methods of the monitor
// 	      i.e., running this function from multiple threads simultaneously MUST be prevented.
{
	dp->state[i] = THINKING ;
	Test(dp, (i+4) % 5) ;
	Test(dp, (i+1) % 5) ;
}

void Test(DiningPhilosophers *dp, int i)
// TO DO: implement this function
{	//pthread_mutex_lock(&dp->mutex) ;
	if((dp->state[(i+4) % 5] != EATING) && (dp->state[i] == HUNGRY) && (dp->state[(i+1) % 5] != EATING))
	{
		dp->state[i] = EATING ;
		if(dp->v_count > 0){
			dp->next_count++ ;
			sem_post(&dp->var) ;
			sem_wait(&dp->next) ;
			dp->next_count-- ;
		}
	}
}

pthread_mutex_t display_mutex = PTHREAD_MUTEX_INITIALIZER;

int CheckPhilosophers(DiningPhilosophers *dp)
// DO NOT modify this function
{
	char status_name[3][16] = { "thinking", "Hungry", "EATING" };
	if(repeat == FALSE)
		return TRUE;

	pthread_mutex_lock(&display_mutex);
	printf("states: ");
	for(int i = 0; i < 5; i++)
		printf("%s ", status_name[dp->state[i]]);
	printf("\n");
	fflush(stdout);
	pthread_mutex_unlock(&display_mutex);

	for(int i = 0; i < 5; i++){
		int prev = (i + 4) % 5;
		int next = (i + 1) % 5;

		if(dp->state[i] == EATING && dp->state[next] == EATING){
			printf("Error! Philosopers %d and %d are eating!\n", i, next);
			exit(-1);
		}
	}

	return TRUE;
}
