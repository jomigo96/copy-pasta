#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h> 
#include <sys/types.h>


int main(int argc, char** argv){
	
	char fifo[10];
	FILE* fp;
	char s[20];
	
	if(argc != 2){
		printf("Input arguments not right. Exiting.\n");
		exit(-1);
	}
	
	strncpy(fifo, argv[1], 9);
	
	fp=fopen(fifo, "w");
	
	if(fp==NULL){
		printf("Error acessing FIFO. Exiting.\n");
		exit(-1);
	}
	strcpy(s, "cat > ");
	strcat(s,fifo);
	
	//fork();
	//fork();
	
	execl("/bin/sh", "sh", "-c", s, NULL);
	
	

	
	
	
	
	exit(0);
}
