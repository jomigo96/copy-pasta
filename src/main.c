#include <stdio.h>
#include "test1.h"

int main(void){
	
	int p1, p2;
	
	while(1){
	
		scanf("%d %d\n", &p1, &p2);
		
		printf("LCD=%d\n", lcd(p1,p2));
		
		/* Hello, this is dog. */
	}
	return 0;
}
