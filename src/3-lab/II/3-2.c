#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(void){
	
	int i=0;
	pid_t pid;
    int t;
	int status;
	
	printf("Creating 10 child processes.\n");
	
	for (i=0; i<10; i++){
		random();
		pid = fork();
		if (pid == 0){
				
			t = random()%10;
			sleep(t);
			return t;
		}else{
			continue;
		}
	}
	
	while(1){
		
		pid = wait(&status);
		printf("RIP child %d. She slept for %d seconds.\n", pid, WEXITSTATUS(status));
			
		random();	
		pid = fork();
		if (pid == 0){
				
			t = random()%10;
			sleep(t);
			return t;
		}
	}
	return 0;
}
