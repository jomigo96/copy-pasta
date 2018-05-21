#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
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

struct clip_data clipboard;

//Secure data structures
pthread_mutex_t m_clip = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m_peers = PTHREAD_MUTEX_INITIALIZER;

//Conditional wait
pthread_cond_t cv_wait = PTHREAD_COND_INITIALIZER;
 
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
	int i;
	int err;
	int client_fd;
	const size_t m_size = sizeof(Message);
	void* buf;
	
	//Threads
	T_param param;
	pthread_t thread_id;
	pthread_t thread_id2;
	
	
	//Assign handler to CTRL-C
	signal(SIGINT, ctrl_c_callback_handler);
	
	
	//Build clipboard
	for (i=0; i<10; i++){
		clipboard.data[i]=NULL;
		clipboard.size[i]=0;
		clipboard.waiting[i]=0;
		clipboard.writing[i]=0;
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
			printf("Error connecting, starting in single mode\n");
				
		}else{
		
			peers.count++;
			peers.sock = realloc(peers.sock, peers.count*sizeof(int));
			peers.sock[peers.count-1]=sock_s_fd;
		 
			//Synchronizing
			printf("Synchronizing repository\n");
			Message r1;
				
			for(i=0; i<10; i++){
				r1.entry=i;
				r1.flag=PASTE;
				
				err = send(sock_s_fd, &r1, m_size, 0);
				if (err <=0 ){
					printf("error syncing\n");
					remove_fd(&peers, sock_s_fd);
					close(sock_s_fd);
					break;
				}
				
				err = recv(sock_s_fd, &r1, m_size, 0);
				if (err <= 0){
					printf("error syncing\n");
					remove_fd(&peers, sock_s_fd);
					close(sock_s_fd);
					break;
				}
				if (r1.flag == NOERROR)
					continue;
						
				clipboard.data[i] = 
							realloc(clipboard.data[i], r1.size);
				if (clipboard.data[i] == NULL){
					perror("realloc");
					exit(-1);
				}
				clipboard.size[i]=r1.size;
				
				buf=clipboard.data[i];
				
				while (r1.size > MESSAGE_SIZE){
				
					memcpy(buf, r1.msg, MESSAGE_SIZE);
					buf = (char*)buf + MESSAGE_SIZE;
					err = recv(sock_s_fd, &r1, m_size, 0);
					if (err <= 0){
						printf("error syncing\n");
						remove_fd(&peers, sock_s_fd);
						close(sock_s_fd);
						break;
					}
				}
				memcpy(buf, r1.msg, r1.size);

				printf("Syncing clipboard position %hi\n", i);
				print_entry(i);
			}
			
		//Launch handler for this peer
		param.sock_id=sock_s_fd;
		param.peers=&peers;
		param.mode=0;
		pthread_create(&thread_id, NULL, thread_1_handler, &param);
		}
	}
	
	
	
	//Create UNIX socket
	sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);	
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}
	
	local_addr.sun_family = AF_UNIX;
	strcpy(local_addr.sun_path, SOCK_ADDRESS);

	err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		unlink(SOCK_ADDRESS);
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
		unlink(SOCK_ADDRESS);
		exit(-1);
	}
	
	// Main loop, accept client connections and launch handler threads
	while(1){ 
		    
		size_addr = sizeof(struct sockaddr);
		
	    client_fd = accept(sock_fd, (struct sockaddr *) & client_addr, &size_addr);
		if(client_fd == -1) {
			perror("accept");
			unlink(SOCK_ADDRESS);
			exit(-1);
		}
		
		printf("Client connected \n");
			
		param.sock_id=client_fd;
		param.peers=&peers;
		param.mode=1;
		
		
		pthread_create(&thread_id, NULL, thread_1_handler, &param);
		
	}	
		
	exit(0);
}



 

































