#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/queue.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#include "clipboard.h"


char sock_address[64];
char* clipboard_data[10];


void ctrl_c_callback_handler(int signum){
	
	int i;
	
	printf("\nCaught signal Ctr-C\n");
	unlink(sock_address);
	
	for(i=0;i<10;i++)
		free(clipboard_data[i]);
	
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
	
	//peer clipboards
	C_peers peers;
	peers.count=0;
	peers.sock=NULL;
	
	//Other variables
	int err;
	int client_fd;
	const size_t m_size = sizeof(Message);
	
	//Threads
	T_param param;
	pthread_t thread_id;
	pthread_t thread_id2;

	
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
		
		peers.count++;
		peers.sock = realloc(peers.sock, peers.count*sizeof(int));
		peers.sock[peers.count-1]=sock_s_fd;
	 
		//Synchronizing
			printf("Synchronizing repository\n");
			Message r1;
			for(i=0; i<10; i++){
				r1.entry=i;
				r1.flag=2;
				send(sock_s_fd, &r1, m_size, 0);  //Need to secure these
				recv(sock_s_fd, &r1, m_size, 0);
				strcpy(clipboard_data[i], r1.msg);
			}
		
		//Launch handler for this peer
		param.sock_id=sock_s_fd;
		param.peers=&peers;
		param.mode=0;
		pthread_create(&thread_id, NULL, thread_1_handler, &param);
	}
	
	
	
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
	
	//Launch remote connection handler thread
	pthread_create(&thread_id2, NULL, thread_2_handler, &peers);
	
	if(listen(sock_fd, 2) == -1) {
		perror("listen");
		unlink(sock_address);
		exit(-1);
	}
	
	
	while(1){ 
		    
		size_addr = sizeof(struct sockaddr);
		
	    client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
		if(client_fd == -1) {
			perror("accept");
			unlink(sock_address);
			exit(-1);
		}
		
		printf("Client connected \n");
			
		param.sock_id=client_fd;
		param.peers=&peers;
		param.mode=1;
		
//		printf("%d \n main sock_id: %d \n %d \n %d \n", param.repository, 
//			param.sock_id, param.mode, param.sock_s_fd);
		
		pthread_create(&thread_id, NULL, thread_1_handler, &param);
		
	}	
		
	exit(0);
}

void * thread_1_handler(void * arg){
	

	int client_fd =  ((T_param*)arg)->sock_id;
	C_peers* peers = ((T_param*)arg)->peers;
	//mode 0-talking with another clipboard; 1-talking with an app
	int mode =       ((T_param*)arg)->mode;
	int err;
	int flag;
	
	Message m;
	const size_t m_size = sizeof(Message);
	int i;
	
	while(1){
		
		err = recv(client_fd, &m, m_size, 0);
		printf("err = %d\n",err);
		if(err == -1){
			printf("ERROR %d\n", errno);
			if(errno==ECONNRESET){
				
				printf("Disconnected\n");
				close(client_fd);
				
				if(mode==PEER_SERVICE){
					remove_fd(peers, client_fd);
				}
			}else{
				perror("recv");
			}
			pthread_exit(NULL);
		}
		if (err == 0 ){ //Assume client disconnected
			close(client_fd);
			printf("Disconnected\n");
			if(mode==PEER_SERVICE){
				remove_fd(peers, client_fd);
			}
			pthread_exit(NULL);
		}

		flag=m.flag;
		
		// also needs to check if the clipboard has enough memory
		if (flag == DISCONNECT){ 
		
			close(client_fd);
			printf("Client disconnected\n");
			if(mode==PEER_SERVICE){
				remove_fd(peers, client_fd);
			}
			pthread_exit(NULL);

		}
		if (flag == COPY){ 
		
			printf("Writing in clipboard position %hi\n", m.entry);
			printf("%s\n",m.msg);
			strcpy(clipboard_data[m.entry], m.msg);
			err = send(client_fd, &m, m_size, 0);
			if (err == -1){
				perror("send");
				if(mode==PEER_SERVICE){
					remove_fd(peers, client_fd);
				}
				close(client_fd);
				pthread_exit(NULL);
			}
			if(peers->count > 0){
				m.flag = REDIRECT;
				
				for(i=0; i<peers->count; i++){
					
					if (client_fd != peers->sock[i]){
						
						printf("Sending to peer fd %d\n", peers->sock[i]);
						
						err = send(peers->sock[i], &m, m_size, 0);
						if (err == -1){
							perror("send");
							remove_fd(peers, peers->sock[i]);
							close(peers->sock[i]);
							i--;
							continue;
						}
					}
				}
			}
		}
		if (flag == PASTE){ //Paste
			
			printf("Sending back clipboard position %hi\n", m.entry);
			strcpy(m.msg, clipboard_data[m.entry]);
			err = send(client_fd, &m, m_size, 0);
			if (err == -1){
				perror("send");
				if(mode==PEER_SERVICE){
					remove_fd(peers, client_fd);
				}
				pthread_exit(NULL);
			}
		}
		if (flag == REDIRECT){
			
			printf("Synchronizing entry %hi\n", m.entry);
			printf("%s\n",m.msg);
			strcpy(clipboard_data[m.entry], m.msg);
			
			for(i=0; i<peers->count; i++){
					
					if (client_fd != peers->sock[i]){
						
						printf("Sending to peer fd %d\n", peers->sock[i]);
						
						err = send(peers->sock[i], &m, m_size, 0);
						if (err == -1){
							perror("send");
							remove_fd(peers, peers->sock[i]);
							close(peers->sock[i]);
							i--;
							continue;
						}
					}
				}
		}
	}
}

void * thread_2_handler(void * arg){
	
	srand((long)pthread_self());
	
	// Internet socket
	struct sockaddr_in local_addr;
	struct sockaddr_in client_addr;
	socklen_t size_addr;
	int sock_fd;
	int client_fd;
	
	//Other variables
	int port=rand()%63714+1024;
	pthread_t thread_id;
	C_peers* peers = (C_peers*)arg;
	T_param param;
	int err;
	
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
	printf("Internet socket created and binded, port %d \n", port);
	
	err = listen(sock_fd, 2);
	if (err == -1){
		perror("listen");
		exit(-1);
	}
	
	while(1){
	
		client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
		if(client_fd == -1) {
			perror("accept");
			exit(-1);
		}
		
		peers->count++;
		peers->sock = realloc(peers->sock, peers->count*sizeof(int));
		peers->sock[peers->count-1]=client_fd;
		
		param.sock_id=client_fd;
		param.peers=peers;
		param.mode=0;
		
		pthread_create(&thread_id, NULL, thread_1_handler, &param);
	}
} 

void remove_fd(C_peers * peerv, int fd){
	
	int i;
	int j=0;

	int * new = malloc( (peerv->count-1)*sizeof(int) );
	
	for(i=0; i < peerv->count; i++){
		
		if(peerv->sock[i] == fd){
		
			j=1;
			
		}else{
			
			if((i == peerv->count-1)&&(j==0)){//The provided fd doesnt exist			
				free(new);
				return;
			}
			new[i-j] = peerv->sock[i]; 
			
		}
	}

	free(peerv->sock);
	peerv->count--;
	peerv->sock = new;
}































