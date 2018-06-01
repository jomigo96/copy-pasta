#include "clipboard-dev.h"

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
#include <sys/wait.h>

extern struct clip_data clipboard;
extern pthread_mutex_t m_clip;
extern pthread_mutex_t m_peers;
extern pthread_cond_t cv_copy;
extern pthread_cond_t cv_wait;
extern pthread_cond_t cv_sync;
extern pthread_mutex_t m_sync;

void * thread_1_handler(void * arg){
	
	int client_fd =  ((T_param*)arg)->sock_id;
	C_peers* peers = ((T_param*)arg)->peers;
	const int mode = ((T_param*)arg)->mode;
	int err;
	int flag;
	int i;
	
	Message m;
	const size_t m_size = sizeof(Message);
	
	void * buf=NULL;
	void * buf2;
	size_t size;
	
	printf("Thread %lu handling client with fd %d\n", pthread_self(), client_fd);
	
	while(1){
		
		err = recv(client_fd, &m, m_size, 0);
		if(err <= 0){
			printf("Disconnected\n");
			close(client_fd);
			if(mode==PEER_SERVICE){
				pthread_mutex_lock(&m_peers);
				remove_fd(peers, client_fd);
				pthread_mutex_unlock(&m_peers);
			}
			pthread_exit(NULL);
		}

		flag=m.flag;
		
		if (flag == DISCONNECT){ 
		
			close(client_fd);
			printf("Client disconnected\n");
			if(mode==PEER_SERVICE){
				pthread_mutex_lock(&m_peers);
				remove_fd(peers, client_fd);
				pthread_mutex_unlock(&m_peers);
			}
			pthread_exit(NULL);

		}
		if ((flag == COPY) || (flag == REDIRECT)){ 
			
			buf=realloc(buf, m.size);			
			if(buf==NULL){
				perror("realloc");
				exit(-1);
			}
			
			size=m.size;
			buf2=buf;
			
			while (m.size > MESSAGE_SIZE){
			
				memcpy(buf2, m.msg, MESSAGE_SIZE);
				buf2 = (char*)buf2 + MESSAGE_SIZE;
				err = recv(client_fd, &m, m_size, 0);
				if (err <= 0){
					printf("Disconnected\n");
					if(mode==PEER_SERVICE){
						pthread_mutex_lock(&m_peers);
						remove_fd(peers, client_fd);
						pthread_mutex_unlock(&m_peers);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
			}
			memcpy(buf2, m.msg, m.size);


			printf("Writing in clipboard position %hi\n", m.entry);
			
			//Critical region
			pthread_mutex_lock(&m_clip);
			clipboard.data[m.entry] = 
						realloc(clipboard.data[m.entry], size);
			if (clipboard.data[m.entry] == NULL){
				perror("realloc");
				exit(-1);
			}
			memcpy(clipboard.data[m.entry], buf, size);
			clipboard.size[m.entry]=size;
			print_entry(m.entry);
			
			//Signal waiting threads
			if (clipboard.waiting[m.entry] > 0){
				clipboard.writing[m.entry]=1;
				pthread_cond_broadcast(&cv_wait);
			}
			pthread_mutex_unlock(&m_clip);
			//End critical region	
			
			
			if (flag != REDIRECT){
				m.flag=NOERROR;
				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					if(mode==PEER_SERVICE){
						pthread_mutex_lock(&m_peers);
						remove_fd(peers, client_fd);
						pthread_mutex_unlock(&m_peers);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
			}
			
			// Replicate to other clipboards - critical region
			pthread_mutex_lock(&m_peers);
			if(peers->count > 0){
				m.flag = REDIRECT;
				buf2=buf;
				
				for(i=0; i<peers->count; i++){
					
					if (client_fd != peers->sock[i]){
						printf("Sending to peer with fd %d\n", 
														peers->sock[i]);
						
						buf=buf2;
						m.size=size;
						
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
				buf=buf2;
			}
			//End critical region
			pthread_mutex_unlock(&m_peers);
		}
		if (flag == PASTE){
			
			printf("Sending back clipboard position %hi\n", m.entry);
			
			//Critical region
			pthread_mutex_lock(&m_clip);
			size=clipboard.size[m.entry];
			if (size == 0){
				pthread_mutex_unlock(&m_clip);
				m.flag=NOERROR;
				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					if(mode==PEER_SERVICE){
						pthread_mutex_lock(&m_peers);
						remove_fd(peers, client_fd);
						pthread_mutex_unlock(&m_peers);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
				continue;
			}else{
				buf=realloc(buf, size);
				if(buf==NULL){
					perror("realloc");
					exit(-1);
				}
				memcpy(buf, clipboard.data[m.entry], size);		
				pthread_mutex_unlock(&m_clip);
			}
			//end critical region
			
			
			buf2=buf;
			m.size=size;
			m.flag=PASTE;
		
			while (m.size > MESSAGE_SIZE){
				
				memcpy(m.msg, buf, MESSAGE_SIZE);

				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					if(mode==PEER_SERVICE){
						pthread_mutex_lock(&m_peers);
						remove_fd(peers, client_fd);
						pthread_mutex_unlock(&m_peers);
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
						pthread_mutex_lock(&m_peers);
						remove_fd(peers, client_fd);
						pthread_mutex_unlock(&m_peers);
					}
					close(client_fd);
					pthread_exit(NULL);
				}
			}
			buf=buf2;
		}
		
		if (flag == WAIT){
			
			pthread_mutex_lock(&m_clip);
			clipboard.waiting[m.entry]++;
			
			while (clipboard.writing[m.entry] != 1){
				pthread_cond_wait(&cv_wait, &m_clip);
			}
			
			size = clipboard.size[m.entry];
			if (size == 0){
				if(0 == (clipboard.waiting[m.entry]--))
					clipboard.writing[m.entry]=0;
				pthread_mutex_unlock(&m_clip);
				m.flag = NOERROR;
				err = send(client_fd, &m, m_size, 0);
				if (err == -1){
					perror("send");
					close(client_fd);
					pthread_exit(NULL);
				}
				
			}else{
				buf = realloc(buf, size);
				memcpy(buf, clipboard.data[m.entry], size);
				
				if(0 == (clipboard.waiting[m.entry]--))
					clipboard.writing[m.entry]=0;
				pthread_mutex_unlock(&m_clip);
				
				
				buf2=buf;
				m.size=size;
			
				while (m.size > MESSAGE_SIZE){
					
					memcpy(m.msg, buf, MESSAGE_SIZE);

					err = send(client_fd, &m, m_size, 0);
					if (err == -1){
						perror("send");
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
						close(client_fd);
						pthread_exit(NULL);
					}
				}
				buf=buf2;
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
	int port=rand()%1000+8000;
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
			if (errno == EINTR)
				continue;
				
			perror("accept");
			exit(-1);
		}
		pthread_mutex_lock(&m_peers);
		peers->count++;
		peers->sock = realloc(peers->sock, peers->count*sizeof(int));
		peers->sock[peers->count-1]=client_fd;
		pthread_mutex_unlock(&m_peers);
		
		param.sock_id=client_fd;
		param.peers=peers;
		param.mode=0;
		
		pthread_create(&thread_id, NULL, thread_1_handler, &param);
	}
}

void * thread_3_handler(void * arg){
	
	C_peers* peers = (C_peers*)arg;
	int i, j;
	Message m;
	void* buf=NULL;
	void* buf2=NULL;
	size_t size;
	const size_t m_size = sizeof(Message);
	int err;
	
	signal(SIGALARM, alarm_done);
	
	
	while(1){
		pthread_mutex_lock(&m_peers);		
		do{
			alarm(30);
			pthread_cond_wait(&cv_sync, &m_peers);
		}while (peers->master != -1);
		
		printf("Timed synchronization\n");
		
		// Replicate everything to other clipboards - critical region
		if(peers->count > 0){
			m.flag = REDIRECT;
			
			for(j=0; j<10; j++){
				
				pthread_mutex_lock(&m_clip);
				size=clipboard.size[j];	
				if(size!=0){
					buf2=realloc(buf2, size);
					if(buf2==NULL){
						perror("realloc");
						exit(-1);
					}
					memcpy(buf2, clipboard.data[j], size);
				}
				pthread_mutex_unlock(&m_clip);
				
				if(size == 0)
					continue;
				
				for(i=0; i<peers->count; i++){
							
					buf=buf2;
					m.size=size;
					m.entry=j;
							
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
		pthread_mutex_unlock(&m_peers);
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
	if(peerv->master == fd){
		peerv->master = -1;
	}
	
	
}

void ctrl_c_callback_handler(int signum){
	
	int i;
	
	printf("\nCaught signal Ctr-C -- exiting\n");
	unlink(SOCK_ADDRESS);
	
	for(i=0;i<10;i++)
		free(clipboard.data[i]);
	
	exit(0);
}

void alarm_done(int signum){
	
	
	pthread_mutex_lock(&m_peers);
	pthread_cond_broadcast(&cv_sync);
	pthread_mutex_unlock(&m_peers);
	signal(SIGALARM, alarm_done);
}
