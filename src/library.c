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
#include <math.h>

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
	
	signal(SIGPIPE, SIG_IGN);
	
	return sock_fd;

}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	
	Message m;
	int err;
	int sent=0;

	if ((region>9)||(region<0))
		return 0;
	
	m.entry = region;
	m.flag = COPY;
	
	while (count > MESSAGE_SIZE){
	
		m.size=count;
	
		memcpy(m.msg, buf, MESSAGE_SIZE);
		
		err = send(clipboard_id, &m, sizeof(Message),0);
		if (err <= 0){
			printf("Error sending message\n");
			return 0;
		}
		sent+=MESSAGE_SIZE;
		buf = (char*)buf + MESSAGE_SIZE;
		count = count - MESSAGE_SIZE;
	}
	
	if (count > 0){
		m.size = count;
		memcpy(m.msg, buf, count);
		err = send(clipboard_id, &m, sizeof(Message),0);
		if (err == -1){
			printf("Error sending message\n");
			return 0;
		}
		sent+=count;
	}
	
	
	err = recv(clipboard_id, &m, sizeof(Message),0);
	if (err == -1){
		printf("Error retreiving response\n");
		return 0;
	}
	if (m.flag == NOERROR)
		return sent;
	else
		return 0;
}
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	
	Message m;
	int err;
	int copied;
	void *aux, *aux2;
		
	if ((region>9)||(region<0))
		return 0;
		
	m.entry = region;
	m.flag = PASTE;
		
		
	err = send(clipboard_id, &m, sizeof(Message),0);
	if (err == -1){
		printf("Error sending message\n");
		return 0;
	}

	err = recv(clipboard_id, &m, sizeof(Message),0);
	if ((err == -1) || (err != sizeof(Message))){
		printf("Error retreiving response\n");
		return 0;
	}
	if(m.flag == NOERROR) //empty entry
		return 0;
		
	aux=malloc(m.size);
	if(aux==NULL){
		perror("malloc");
		return 0;
	}
	aux2=aux;
	
	copied = (count >= m.size) ? m.size : count;
	
	while (m.size > MESSAGE_SIZE){

		memcpy(aux2, m.msg, MESSAGE_SIZE);

		err = recv(clipboard_id, &m, sizeof(Message),0);
		if ((err == -1) || (err != sizeof(Message))){
			printf("Error retreiving response\n");
			return 0;
		}
		aux2 = (char*)aux2 + MESSAGE_SIZE;
		
	}
	
	memcpy(aux2, m.msg, m.size);
	memcpy(buf, aux, copied);
	free(aux);
	
	return copied;
}

int clipboard_wait(int clipboard_id, int region, void* buf, size_t count){
	
	Message m;
	int err;
	int copied;
	void *aux, *aux2;
		
	if ((region>9)||(region<0))
		return 0;
		
	m.entry = region;
	m.flag = WAIT;
		
	err = send(clipboard_id, &m, sizeof(Message),0);
	if (err == -1){
		printf("Error sending message\n");
		return 0;
	}

	err = recv(clipboard_id, &m, sizeof(Message),0);
	if ((err == -1) || (err != sizeof(Message))){
		printf("Error retreiving response\n");
		return 0;
	}
	if(m.flag == NOERROR) //empty entry
		return 0;
		
	aux=malloc(m.size);
	if(aux==NULL){
		perror("malloc");
		return 0;
	}
	aux2=aux;
	
	copied = (count >= m.size) ? m.size : count;
	
	while (m.size > MESSAGE_SIZE){

		memcpy(aux2, m.msg, MESSAGE_SIZE);

		err = recv(clipboard_id, &m, sizeof(Message),0);
		if ((err == -1) || (err != sizeof(Message))){
			printf("Error retreiving response\n");
			return 0;
		}
		aux2 = (char*)aux2 + MESSAGE_SIZE;
		
	}
	
	memcpy(aux2, m.msg, m.size);
	memcpy(buf, aux, copied);
	free(aux);
	
	return copied;	
	
}


void clipboard_close(int clipboard_id){
	
	Message m;
	
	m.flag = DISCONNECT;
	
	send(clipboard_id, &m, sizeof(Message), 0);
	close(clipboard_id);
}







