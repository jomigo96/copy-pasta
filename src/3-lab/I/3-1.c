#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(void){
	
	int i;
	pid_t pid;
	long int t;
	unsigned long int seed;
	
	for(i=0 ; i<10; i++){
		
		pid = fork();
		if ( pid == 0 ){
			
			t = random()%10;
			sleep(t);
			printf("I am a child process, my pid is %d and I was asleep for %d seconds.\n", getpid(), t);	
			exit(0);
			
		}else{
			
			seed = random();
			srand(seed);
			 
			continue;
		}
	}
	getchar();

	return 0;
}
