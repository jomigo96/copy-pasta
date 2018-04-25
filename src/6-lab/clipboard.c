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

#include "clipboard.h"

void ctrl_c_callback_handler(int signum){
	printf("Caught signal Ctr-C\n");
	unlink(SOCK_ADDRESS);
	exit(0);
}

 
int main(){

	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	 socklen_t size_addr;
	
	char* clipboard_data[10];
	
	//Assign handler to CTRL-C
	signal(SIGINT, ctrl_c_callback_handler);
	
	//Create socket
	int sock_fd= socket(AF_UNIX, SOCK_STREAM, 0);	
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		unlink(SOCK_ADDRESS);
		printf("Unlinked previous address.\n");
		err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
		if(err == -1) {
			perror("bind");
			exit(-1);
		}
	}
	printf(" socket created and binded \n");
	
	if(listen(sock_fd, 2) == -1) {
		perror("listen");
		unlink(SOCK_ADDRESS);
		exit(-1);
	}
	
	//Build clipboard
	int i;
	for (i=0; i<10; i++){
		clipboard_data[i]=malloc(POSITION_SIZE*sizeof(char));
		if (clipboard_data[i] == NULL){
			printf("Error allocating clipboard memory.\n");
			unlink(SOCK_ADDRESS);
			exit(-1);
		}
	}
	
	const size_t m_size = sizeof(Message);
	
	char* data;
	
	data = malloc(m_size);
	if(data == NULL){
		printf("Error allocating memory.\n");	
		unlink(SOCK_ADDRESS);
		exit(-1);	
	}
	
	//Accept connection
		int client_fd= accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
		if(client_fd == -1) {
			perror("accept");
			unlink(SOCK_ADDRESS);
			exit(-1);
		}
		printf("Client connected\n");
	
	
	Message m, m2;
	
	while(1){
		
		
		
		
		printf(".\n");

		err = recv(client_fd, &m, m_size, 0);
		if(err == -1){
			perror("recv");
			unlink(SOCK_ADDRESS);
			exit(-1);
		}
//		printf("Read something\n");
		m2=m;
		
		// also needs to check if the clipboard has enough memory
		if (m.flag == 0){
		
			close(client_fd);
			printf("Client disconnected\n");
			
			client_fd= accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
			if(client_fd == -1) {
				perror("accept");
				unlink(SOCK_ADDRESS);
				exit(-1);
			}
			printf("Client connected\n");
		}
		if (m.flag == 1){
		
			printf("Writing in clipboard position %hi\n", m.entry);
			printf("%s\n",m.msg);
			strcpy(clipboard_data[m.entry], m.msg);
			err = send(client_fd, &m, m_size, 0);
		}
		if (m.flag == 2){
			
			printf("Sending back clipboard position %hi\n", m.entry);
			strcpy(m2.msg, clipboard_data[m.entry]);
			err = send(client_fd, &m2, m_size, 0);
		}	
		if (err == -1){
			perror("send");
			unlink(SOCK_ADDRESS);
			exit(-1);
		}
		

	}
		
	exit(0);
	
}
