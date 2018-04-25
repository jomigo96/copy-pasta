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

#include "clipboard.h"

char sock_address[64];

void ctrl_c_callback_handler(int signum){
	printf("Caught signal Ctr-C\n");
	unlink(sock_address);
	exit(0);
}

 
int main(int argc, char** argv){
	
	// Unix socket
	struct sockaddr_un local_addr;
	struct sockaddr_un client_addr;
	socklen_t size_addr;
	int sock_fd;
	// Internet socket
	struct sockaddr_in server_addr;
	int sock_s_fd;
	
	//Other variables
	int mode=-1;
	char* clipboard_data[10];
	const size_t m_size = sizeof(Message);
	char* data;
	int err;
	
	//sprintf(sock_address, "%s%d", SOCK_ADDRESS, getpid());
	strcpy(sock_address, SOCK_ADDRESS);
	
	//Assign handler to CTRL-C
	signal(SIGINT, ctrl_c_callback_handler);
	
	
	//Build clipboard
	int i;
	for (i=0; i<10; i++){
		clipboard_data[i]=malloc(POSITION_SIZE*sizeof(char));
		if (clipboard_data[i] == NULL){
			printf("Error allocating clipboard memory.\n");
			unlink(sock_address);
			exit(-1);
		}
	}
	
	
	
	// Connect to remote repository
	if((argc == 4) && !(strcmp("-c", argv[1]))){
		printf("Starting clipboard in connected mode.\n");
		mode=1;
		
		sock_s_fd = socket(AF_INET, SOCK_STREAM, 0);
	
		if (sock_s_fd == -1){
			perror("socket: ");
			exit(-1);
		}
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(atoi(argv[3]));
		inet_aton(argv[2], &server_addr.sin_addr);
		
		if( -1 == connect(sock_s_fd, 
			(const struct sockaddr *) &server_addr, 
			sizeof(server_addr))){
				printf("Error connecting\n");
				exit(-1);
		}
	 
		//Synchronizing
		if(mode==1){
			printf("Synchronizing repository\n");
			Message r1;
			for(i=0; i<10; i++){
				r1.entry=i;
				r1.flag=2;
				send(sock_s_fd, &r1, m_size, 0);  //Need to secure these
				recv(sock_s_fd, &r1, m_size, 0);
				strcpy(clipboard_data[i], r1.msg);
			}
		}
	}
	
/*	//Start in single mode
	if ((argc == 1)||(mode == 0)){
	}*/
	
	//Create UNIX socket
	sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);	
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, sock_address);

	err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		unlink(sock_address);
		printf("Unlinked previous address.\n");
		err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
		if(err == -1) {
			perror("bind");
			exit(-1);
		}
	}
	printf("socket created and binded \n");
	
	data = malloc(m_size);
	if(data == NULL){
		printf("Error allocating memory.\n");	
		unlink(sock_address);
		exit(-1);	
	}
	
	if(listen(sock_fd, 2) == -1) {
		perror("listen");
		unlink(sock_address);
		exit(-1);
	}
	
	
	
	//Accept connection
		int client_fd= accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
		if(client_fd == -1) {
			perror("accept");
			unlink(sock_address);
			exit(-1);
		}
		printf("Client connected\n");
		
	
	
	Message m, m2;
	
	while(1){
		
		printf(".\n");

		err = recv(client_fd, &m, m_size, 0);
		if(err == -1){
			perror("recv");
			unlink(sock_address);
			exit(-1);
		}
//		printf("Message: %s\n",m.msg);
//		printf("Read something\n");
		m2=m;
		
		// also needs to check if the clipboard has enough memory
		if (m.flag == 0){
		
			close(client_fd);
			printf("Client disconnected\n");
			
			client_fd= accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
			if(client_fd == -1) {
				perror("accept");
				unlink(sock_address);
				exit(-1);
			}
			printf("Client connected\n");
		}
		if (m.flag == 1){
		
			printf("Writing in clipboard position %hi\n", m.entry);
			printf("%s\n",m.msg);
			strcpy(clipboard_data[m.entry], m.msg);
			err = send(client_fd, &m, m_size, 0);
			if (err == -1){
				perror("send");
				unlink(sock_address);
				exit(-1);
			}
			if(mode == 1){
				err = send(sock_s_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					unlink(sock_address);
					exit(-1);
				}
			}
		}
		else if (m.flag == 2){
			
			printf("Sending back clipboard position %hi\n", m.entry);
			strcpy(m2.msg, clipboard_data[m.entry]);
			err = send(client_fd, &m2, m_size, 0);
			if (err == -1){
				perror("send");
				unlink(sock_address);
				exit(-1);
			}
		}	
		
		

	}
		
	exit(0);
	
}
