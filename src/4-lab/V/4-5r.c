

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
	char* s=NULL;
	size_t n=0;
	size_t r;
	int i;
	
	if(argc != 2){
		printf("Input arguments not right. Exiting.\n");
		exit(-1);
	}
	
	strncpy(fifo, argv[1], 9);
	
	fp=fopen(fifo, "r");
	
	if(fp==NULL){
		printf("Error acessing FIFO. Exiting.\n");
		exit(-1);
	}
	
	while(1){
		r=getline(&s, &n, fp);
		
		if(r!=-1){
			for(i=0; i<n; i++){
				s[i]=toupper(s[i]);
			}
			printf("%s",s);
		}
		
	}
	
	
	
	exit(0);
}

