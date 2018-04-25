/*Compile with: 
 * 	gcc -c 4-1.c
 *  gcc 4-1.o -lm -o 4-1
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h> 
#include <sys/types.h>

#define V_SIZE 100

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


int main(int argc, char** argv){
	
	int slave_count;
	int sequence_length;
	int i;
	pid_t pid=1;
	int fd[2];
	int vec[V_SIZE];
	int prime_count;
	
	
	if(argc != 3){
		printf("Input arguments not right. Exiting.\n");
		exit(-1);
	}
	
	slave_count = strtol(argv[1], NULL, 10);
	sequence_length = strtol(argv[2], NULL, 10);
	
	pipe(fd);
	
	for(i=0; i<slave_count; i++){
		if(pid!=0)
			pid=fork();
	}

	
	while(1){
		
		if(pid==0){
			
			while(1){
			
				read( fd[0], vec, V_SIZE*sizeof(int) );
				
				for(i=0; vec[i]!=0; i++)
					;
				sequence_length=i+1;
				
				prime_count=sequence_length;
			
				for(i=0 ; i<sequence_length; i++){
				
					if(!(is_prime(vec[i])))
						prime_count--;			
				}
				printf("Child %d counted %d primes.\n", getpid(), prime_count);
			}
			
		}else{
			
			for(i=0; i<sequence_length; i++){
				
				vec[i]=random()%100000;
				printf("Writing the number %d\n",vec[i]);
			}
			for( ; i<V_SIZE; i++){
				vec[i]=0;
			}
			write(fd[1], vec, V_SIZE*sizeof(int));
			getchar();
		}
	}
	exit(0);
}


