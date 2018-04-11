#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clipboard.h"


 
int main(){
	char file_name[100];
	
	char* clipboard_data[10];
	
	sprintf(file_name, "./%s", OUTBOUND_FIFO);
	unlink(file_name);
	if(mkfifo(file_name, 0666)==-1){
		printf("Error creating out fifo\n");
		exit(-1);
	}
	int fifo_out = open(file_name, O_RDWR);
	if(fifo_out == -1){
		printf("Error opening in fifo\n");
		exit(-1);
	}
	
	
	
	sprintf(file_name, "./%s", INBOUND_FIFO);
	unlink(file_name);
	if(mkfifo(file_name, 0666)==-1){
		printf("Error creating in fifo\n");
		exit(-1);
	}
	int fifo_in = open(file_name, O_RDWR);
	if(fifo_in == -1){
		printf("Error opening in fifo\n");
		exit(-1);
	}

	//criar FIFOS
	
	//abrir FIFOS
	
	//Build clipboard
	int i;
	for (i=0; i<10; i++){
		clipboard_data[i]=malloc(POSITION_SIZE*sizeof(char));
		if (clipboard_data[i] == NULL){
			printf("Error allocating clipboard memory.\n");
			exit(-1);
		}
	}
	
	const size_t m_size = sizeof(Message);
	
	char* data;
	
	data = malloc(m_size);
	if(data == NULL){
		printf("Error allocating memory.\n");	
		exit(-1);	
	}
	
	Message m, m2;
	
	while(1){
		
		printf(".\n");
		read(fifo_in, &m, m_size);
//		printf("Read something\n");
		m2=m;
		
		// also needs to check if the clipboard has enough memory
		if (m.flag == 1){
		
			printf("Writing in clipboard position %hi\n", m.entry);
			printf("%s\n",m.msg);
			strcpy(clipboard_data[m.entry], m.msg);
			write(fifo_out, &m, m_size);
		}
		if (m.flag == 2){
			
			
			
			printf("Sending back clipboard position %hi\n", m.entry);
			strcpy(m2.msg, clipboard_data[m.entry]);
			write(fifo_out, &m2, m_size);
		}	

		

	}
		
	exit(0);
	
}
