#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "clipboard.h"



void ctrl_c_callback_handler(int signum){
	printf("Caught signal Ctr-C\n");
	exit(0);
}

 
int main(int argc, char** argv){
	srand(getpid());
	

	// Internet socket
	struct sockaddr_in local_addr;
	struct sockaddr_in client_addr;
	socklen_t size_addr;
	int sock_fd;
	int client_fd;
	
	//Other variables
	char* clipboard_data[10];
	int port=rand()%63714+1024;
	const size_t m_size = sizeof(Message);
	char* data;
	int err;
	
	//Assign handler to CTRL-C
	signal(SIGINT, ctrl_c_callback_handler);
	
	
	//Build clipboard
	int i;
	for (i=0; i<10; i++){
		clipboard_data[i]=malloc(POSITION_SIZE*sizeof(char));
		if (clipboard_data[i] == NULL){
			printf("Error allocating clipboard memory.\n");
			exit(-1);
		}
	}
	data = malloc(m_size);
	if(data == NULL){
		printf("Error allocating memory.\n");	
		exit(-1);	
	}
	
	
	printf("Starting clipboard in backup mode\nPort: %d\n", port);
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(port);
	local_addr.sin_addr.s_addr= INADDR_ANY;
	err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}
	printf("Internet socket created and binded \n");
	
	err = listen(sock_fd, 2);
	if (err == -1){
		perror("listen");
		exit(-1);
	}
	
	Message m, m2;
	
	client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
	if(client_fd == -1) {
		perror("accept");
		exit(-1);
	}
	
	
	while(1){

		printf(".\n");

		err = recv(client_fd, &m, m_size, 0);
		if(err == -1){
			if (errno==ECONNRESET){
				close(client_fd);
				client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
				if(client_fd == -1) {
					perror("accept");
					exit(-1);
				}
			}else{
				perror("recv");
				exit(-1);
			}
		}
		if (err == 0){
			close(client_fd);
			client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
			if(client_fd == -1) {
				perror("accept");
				exit(-1);
			}
		}
//		printf("%d\n",err);
		m2=m;

		
		// also needs to check if the clipboard has enough memory
		if (m.flag == 1){
			printf("Writing in clipboard position %hi\n", m.entry);
			printf("%s\n",m.msg);
			strcpy(clipboard_data[m.entry], m.msg);
		}
		if (m.flag == 2){
			printf("Sending back clipboard position %hi\n", m.entry);
			strcpy(m2.msg, clipboard_data[m.entry]);
			err = send(client_fd, &m2, m_size, 0);
			if (err == -1){
				perror("send");
				exit(-1);
			}
		}	
		
		
	}
		
	exit(0);
	
}
