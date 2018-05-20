#include "clipboard.h"

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

extern struct clip_data clipboard;



void * thread_1_handler(void * arg){
	

	int client_fd =  ((T_param*)arg)->sock_id;
	C_peers* peers = ((T_param*)arg)->peers;
	const int mode = ((T_param*)arg)->mode;
	int err;
	int flag;
	int i;
	
	Message m;
	const size_t m_size = sizeof(Message);
	
	void * buf;
	
	printf("Thread %lu handling client with fd %d\n", pthread_self(), client_fd);
	
	while(1){
		
		err = recv(client_fd, &m, m_size, 0);
		if(err <= 0){
			printf("Disconnected\n");
			close(client_fd);
			if(mode==PEER_SERVICE){
				remove_fd(peers, client_fd);
			}
			pthread_exit(NULL);
		}

		flag=m.flag;
		
		if (flag == DISCONNECT){ 
		
			close(client_fd);
			printf("Client disconnected\n");
			if(mode==PEER_SERVICE){
				remove_fd(peers, client_fd);
			}
			pthread_exit(NULL);

		}
		if ((flag == COPY) || (flag == REDIRECT)){ 
			
						
			clipboard.data[m.entry] = 
						realloc(clipboard.data[m.entry], m.size);
			if (clipboard.data[m.entry] == NULL){
				perror("realloc");
				exit(-1);
			}
			clipboard.size[m.entry]=m.size;
			
			buf=clipboard.data[m.entry];
			
			while (m.size > MESSAGE_SIZE){
			
				memcpy(buf, m.msg, MESSAGE_SIZE);
				buf = (char*)buf + MESSAGE_SIZE;
				err = recv(client_fd, &m, m_size, 0);
				if ((err == -1) || (err == 0)){
					printf("Disconnected\n");
					if(mode==PEER_SERVICE){
						remove_fd(peers, client_fd);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
			}
			memcpy(buf, m.msg, m.size);

			printf("Writing in clipboard position %hi\n", m.entry);
			print_entry(m.entry);
			
			if (flag != REDIRECT){
				m.flag=NOERROR;
				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					if(mode==PEER_SERVICE){
						remove_fd(peers, client_fd);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
			}
			
			// Replicate to other clipboards
			if(peers->count > 0){
				m.flag = REDIRECT;
				
				for(i=0; i<peers->count; i++){
					
					if (client_fd != peers->sock[i]){
						printf("Sending to peer with fd %d\n", 
														peers->sock[i]);
						
						buf=clipboard.data[m.entry];
						m.size=clipboard.size[m.entry];
						memcpy(m.msg, buf, MESSAGE_SIZE);
						
						while (m.size > MESSAGE_SIZE){
							
							
								memcpy(m.msg, buf, MESSAGE_SIZE);

								err = send(peers->sock[i], &m,m_size,0);
								if (err == -1){
									perror("send");
									remove_fd(peers, peers->sock[i]);
									close(peers->sock[i]);
									i--;
									continue;
								}
						
								m.size = m.size - MESSAGE_SIZE;
								buf = (char*)buf + MESSAGE_SIZE;
							
						}
						if(m.size > 0){
							memcpy(m.msg, buf, m.size);
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
		if (flag == PASTE){
			
			printf("Sending back clipboard position %hi\n", m.entry);
			
			if (clipboard.size[m.entry] == 0){
				m.flag=NOERROR;
				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					if(mode==PEER_SERVICE){
						remove_fd(peers, client_fd);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
				continue;
			}
			
			buf=clipboard.data[m.entry];
			m.size=clipboard.size[m.entry];
			m.flag=PASTE;
			memcpy(m.msg, buf, MESSAGE_SIZE);
		
			while (m.size > MESSAGE_SIZE){
				
				memcpy(m.msg, buf, MESSAGE_SIZE);

				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					if(mode==PEER_SERVICE){
						remove_fd(peers, client_fd);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
				
				m.size = m.size - MESSAGE_SIZE;
				buf = (char*)buf + MESSAGE_SIZE;
			}
			if(m.size > 0){
				memcpy(m.msg, buf, m.size);
				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					if(mode==PEER_SERVICE){
						remove_fd(peers, client_fd);
					}
					close(client_fd);
					pthread_exit(NULL);
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

void print_entry(int entry){
	
	int i;
	
	for(i=0; i<clipboard.size[entry]; i++)
		putchar( ((char*)clipboard.data[entry])[i] );
		
	
	putchar('\n');
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

void ctrl_c_callback_handler(int signum){
	
	int i;
	
	printf("\nCaught signal Ctr-C -- exiting\n");
	unlink(SOCK_ADDRESS);
	
	for(i=0;i<10;i++)
		free(clipboard.data[i]);
	
	exit(0);
}
