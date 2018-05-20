#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(){

		int fd = clipboard_connect("./");
		int opt=1;
		int entry;
		int code;
		
		
		
		if(fd== -1){
			exit(-1);
		}
		char *dados;
		size_t size = 0;


		dados=malloc(128);
		size=128;
		
		
/*		int vec[5]={0b10, 0, 420, -128, 168598};
		printf("%d bytes were sent as integers.\n", clipboard_copy(fd, 0, vec, 5*sizeof(int)));
		memset(vec, 0, sizeof(int)*5);
		printf("Vec: %d %d %d %d %d\n", vec[0], vec[1],vec[2],vec[3],vec[4]);
		clipboard_paste(fd, 0, vec, 3*sizeof(int));
		printf("Vec: %d %d %d %d %d\n", vec[0], vec[1],vec[2],vec[3],vec[4]);
*/		
		while(opt != 0){
			
			printf("Copy - 1  Paste - 2   Exit - 0\n");
			scanf("%d", &opt);
			while(getchar() != '\n');
			
			if (opt == 1){
				printf("Message: ");
				getline(&dados, &size, stdin);	
				printf("Entry: ");
				scanf("%d", &entry);
				while(getchar() != '\n')
					;
				
				//printf("Calling copy with [%s]\n", dados);
				
				code=clipboard_copy(fd, entry, dados, strlen(dados)+1);
				if(code > 0)
					printf("Success, sent %d bytes\n", code);
				else
					printf("Failure\n");
					
				memset(dados, 0, size);
			}
			if (opt == 2){
				printf("Entry: ");
				scanf("%d", &entry);
				while(getchar() != '\n')
					;
				code=clipboard_paste(fd, entry, dados, size);
				if(code > 0){
					printf("Success, received %d bytes\n", code);
					printf("Message: %s\n",dados);
				}
				else
					printf("Failure\n");
			}
			
		}
		clipboard_close(fd);
		free(dados);
		exit(0);
	}
