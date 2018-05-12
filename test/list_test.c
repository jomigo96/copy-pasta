#include <stdio.h>
#include "list.h"


int main(void){
	
	Client* client_list;
	Client* other;
	int a;
	
	
	client_list = client_init();
	printf("Client %lu --- next ptr %lu --- value %d\n",client_list,
		client_list->next, client_list->fd);
		
		
	printf("Inseting element\n");
	
	a=10;
	client_pushback(client_list,a);
	printf("Client %lu --- next ptr %lu --- value %d\n",client_list,
		client_list->next, client_list->fd);
	other=client_list->next;
	printf("Client %lu --- next ptr %lu --- value %d\n",other,
		other->next, other->fd);
		
	printf("Inseting element\n");
	
	a=15;
	client_pushback(client_list,a);
	printf("Client %lu --- next ptr %lu --- value %d\n",client_list,
		client_list->next, client_list->fd);
	other=client_list->next;
	printf("Client %lu --- next ptr %lu --- value %d\n",other,
		other->next, other->fd);
	other=other->next;
	printf("Client %lu --- next ptr %lu --- value %d\n",other,
		other->next, other->fd);
	
	printf("Removing element\n");
	
	client_remove(client_list, 10);
	printf("Client %lu --- next ptr %lu --- value %d\n",client_list,
		client_list->next, client_list->fd);
	other=client_list->next;
	printf("Client %lu --- next ptr %lu --- value %d\n",other,
		other->next, other->fd);
		
	client_delete(client_list);
	
return 0;	
}
