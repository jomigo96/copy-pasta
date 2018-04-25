#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>


int main(void){
	
	FILE* fp;
	char* line=NULL;
	size_t n=0;
	pid_t pid;
	int status;
	
	clockid_t clk_id = CLOCK_REALTIME;
	struct timespec t_start, t_end;

	long time;
	
	fp=fopen("command_list.txt", "r");
	if(fp==NULL){
		
		printf("Error opening file\n");
		return 0;
	}
	
	while(getline(&line, &n,fp) != -1){
		
		//printf("%s",line);
		
		clock_gettime(clk_id, &t_start);
		pid=fork();
		if (pid == 0){
			
			execl("/bin/sh", "sh", "-c", line, NULL);
			
		}else{
			
			waitpid(pid, &status, 0);
			clock_gettime(clk_id, &t_end);
			
			time = (t_end.tv_sec - t_start.tv_sec)*1E9 + (t_end.tv_nsec-t_start.tv_nsec);
			
			
			printf("Command %s ran for %ld nano-seconds\n", line, time);
		}	
	}
	
	printf("Program ended\n");
	fclose(fp);
	return 0;
}



