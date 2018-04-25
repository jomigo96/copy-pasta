#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#define SIGALARM 14

int i=0;

void alarm_done();

int main(void){
	
	struct sigaction sa;
	
	sa.sa_handler = alarm_done;
	sigaction(SIGALARM, &sa, NULL);
		
	alarm( (random()%5)+1 );
	
	while(1){
		
		printf("%d\n",i++);
		usleep(500);
		
	}
	
	
	return 0;
}

void alarm_done(){
	
	int t;
	
	t=random()%10;
	alarm(t+1);
	i=0;
}
