#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <pthread.h>

int fd_1[2];

int is_prime(int n){
	
	int i; 
	
	if(((n%2)==0)||(n==1))
		return 0;
		
	for(i=3; i<=ceil(sqrt(n)); i+=2){
		
		if((n%i)==0)
			return 0;
			
	}
	return 1;
}

void * thread_handler(void * data){

	*(int*)data=0;
	int n;

	while(1){
		
		read(fd_1[0], &n, sizeof(int));
		*(int*)data += is_prime(n);
	}
}



int main(int argc, char** argv){
	
	int i;
	pthread_t* threads;
	int thread_number;
	int* returns;
	int a;
	
	
	if(argc != 3){
		printf("Arguments insufficient\n");
		exit(-1);
	}
	
	thread_number=atoi(argv[1]);
	threads=malloc(thread_number*sizeof(pthread_t));
	returns=malloc(thread_number*sizeof(int));
	
	srand(getpid());
	
	pipe(fd_1);
	
	for(i=0; i<thread_number; i++){
		pthread_create(&threads[i], NULL, thread_handler, &returns[i]); 
	}
	
	for(i=atoi(argv[2]); i>0; i--){
		a=rand();
		write(fd_1[1], &a, sizeof(int));
	}
	
	getchar();
	
	a=0;
	for(i=atoi(argv[1])-1; i>=0; i--){
		pthread_cancel(threads[i]);
		a+=returns[i];
	}
	free(threads);
	free(returns);
	
	printf("Total number of primes: %d\n", a);
	
exit(0);	
}
