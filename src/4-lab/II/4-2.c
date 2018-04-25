/*Compile with: 
 * 	gcc -c 4-2.c
 *  gcc 4-2.o -lm -o 4-2
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
	int fd_1[2];
	int fd_2[2];
	int vec[V_SIZE];
	int prime_count=0;
	
	
	if(argc != 3){
		printf("Input arguments not right. Exiting.\n");
		exit(-1);
	}
	
	slave_count = strtol(argv[1], NULL, 10);
	sequence_length = strtol(argv[2], NULL, 10);
	
	pipe(fd_1);
	pipe(fd_2);
	
	for(i=0; i<slave_count; i++){
		if(pid!=0)
			pid=fork();
	}

	
	while(1){
		
		if(pid==0){
			
			read( fd_1[0], vec, V_SIZE*sizeof(int) );
				
			prime_count=sequence_length;
			
			for(i=0 ; i<sequence_length; i++){
				
				if(!(is_prime(vec[i])))
					prime_count--;			
			}
			write(fd_2[1], &prime_count, sizeof(int));
			
		}else{
			
			for(i=0; i<sequence_length; i++){
				
				vec[i]=random()%100000;
				printf("Writing the number %d\n",vec[i]);
			}
			for( ; i<V_SIZE; i++){
				vec[i]=0;
			}
			write(fd_1[1], vec, V_SIZE*sizeof(int));
			
			read(fd_2[0], &i, sizeof(int));
			prime_count+=i;
			printf("Partial prime count: %d\n", prime_count);
			getchar();
			
		}
	}
	exit(0);
}


