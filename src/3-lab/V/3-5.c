#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


int main(void){
	
	int i;
	pid_t pid;
	long int t, time;
	
	clockid_t clk_id = CLOCK_REALTIME;
	struct timespec t_start, t_end;
	
	for(i=0 ; i<10; i++){
		
		random();
		pid = fork();
		if ( pid == 0 ){
			
			t = random()%10+1;
			
			clock_gettime(clk_id, &t_start);
			sleep(t);
			clock_gettime(clk_id, &t_end);
			
			time = (t_end.tv_sec - t_start.tv_sec)*1E9 + (t_end.tv_nsec-t_start.tv_nsec);
			
			printf("I am a child process, my pid is %d and I was asleep for %ld seconds, or %f seconds, depending who you ask\n", getpid(), t, (float)time/1E9);	
			exit(0);
			
		
		}
	
	}
	getchar();
	return 0;
}
