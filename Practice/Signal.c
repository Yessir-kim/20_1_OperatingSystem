#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void sig_handler(int signo) ;

int main()
{
	int i = 0 ;
	/***************************************************************

		When signal is occured, call sig_handler.
		If you want to use init setting, use SIG_DFL or SIG_IGN flag
	
	****************************************************************/
	signal(SIGINT, sig_handler) ; // or signal(SIGINT, SIG_DFL) ; 

	while(1)
	{
		printf("%d\n", i++) ;
		sleep(1) ;
	}

	return 1 ;
}

void sig_handler(int signo)
{
	printf("SIGINT wad received!\n") ;
	printf("one more time!\n") ;
	signal(SIGINT, SIG_DFL) ;
}
