#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main(){
	
		int fd1 = clipboard_connect("./dir1/");
		int fd2 = clipboard_connect("./dir2/");
		
		char M1[10] = "AAAAAAAAAA";
		char M2[10] = "BBBBBBBBBB";
		
		M1[9] = '\0';
		M2[9] = '\0';
		char dados1[10];
		char dados2[10];
		
		if((fd1 == -1)||(fd2 == -1)){
			exit(-1);
		}


		printf("Copying messages\n");
		
		clipboard_copy(fd1, 9, M1, 10);
		clipboard_copy(fd2, 9, M2, 10);
		
		while(1){
			printf("Requesting back messages\n");
			clipboard_paste(fd1, 9, dados1, 10);
			clipboard_paste(fd2, 9, dados2, 10);
			printf("On dir1: %s --- On dir2: %s\n ", dados1, dados2);
			printf("Press Enter to retry\n");
			while(getchar()!='\n')
				;
		}


		clipboard_close(fd1);
		clipboard_close(fd2);
		exit(0);
	}
