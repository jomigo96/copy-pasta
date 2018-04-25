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

int clipboard_connect(char * clipboard_dir){

	struct sockaddr_un server_addr;
	struct sockaddr_un client_addr;
	
	int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock_fd == -1){
		printf("Error creating socket.\n");
		return -1;
	}

	printf(" socket created \n");

	client_addr.sun_family = AF_UNIX;
	sprintf(client_addr.sun_path, "%ssocket_%d", clipboard_dir ,getpid());
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCK_ADDRESS);

	int err_c = connect(sock_fd, (const struct sockaddr *) &server_addr, 
							sizeof(server_addr));
	if(err_c==-1){
		printf("Error connecting.\n");
		return -1;
	}
	
	printf("connected to clipboard\n");
	return sock_fd;

}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	
	Message m;

	if ((count-1) > sizeof(char)*MESSAGE_SIZE)
		return -1;
	if ((region>9)||(region<0))
		return -1;
	
	m.entry = region;
	m.flag = 1;
	
	
	strcpy(m.msg, (char*)buf);
	

	//printf("%s\n",m.msg);
	int err;
		
	err = send(clipboard_id, &m, sizeof(Message),0);
	if (err == -1){
		printf("Error sending message\n");
		return -1;
	}
	err = recv(clipboard_id, &m, sizeof(Message),0);
	if (err == -1){
		printf("Error retreiving response\n");
		return -1;
	}
	
	if (m.flag == 1)
		return 1;
	else
		return -1;
	
}
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	
	Message m;
	int m_size;	
	
		
	if ((region>9)||(region<0))
		return -1;
		
	m.entry = region;
	m.flag = 2;
		
	int err;	
		
	err = send(clipboard_id, &m, sizeof(Message),0);
	if (err == -1){
		printf("Error sending message\n");
		return -1;
	}
	err = recv(clipboard_id, &m, sizeof(Message),0);
	if (err == -1){
		printf("Error retreiving response\n");
		return -1;
	}
	
	if (m.flag == 2){
		m_size=strlen(m.msg)+1;
		
		if(m_size > count){
			printf("Error: buffer size insufficient.\n");
			return -1;
		}
			
		//printf("%s\n",m.msg);
			
		strcpy((char*)buf, m.msg);
		return m_size;
		
	}
	else
		return -1;
	
}

void clipboard_close(int clipboard_id){
	
	Message m;
	
	m.flag = 0;
	
	int err = send(clipboard_id, &m, sizeof(Message), 0);
	if (err == -1){
		printf("Error sending message\n");
		exit(-1);
	}
	close(clipboard_id);
}
