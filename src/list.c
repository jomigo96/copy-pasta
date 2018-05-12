#include "list.h"
#include <stdlib.h>
#include <stdio.h>

Client* client_init(void){
	
	Client * c;
	
	if ((c = malloc(sizeof(Client))) == NULL ){
		perror("malloc: ");
		exit(-1);
	}
	c->fd = -1;
	c->next=NULL;
	return c;
}

Client* client_remove(Client* head, int fd){
	
	Client* prev;
	Client* r=head;
	
	if(fd == -1) //attempting to remove head
		return head;
	
	while ( fd != r->fd ){
		
		prev=r;
		if((r=r->next)==NULL)
			return head;
	}
	
	prev->next=r->next;
	free(r);
	return head;
	
}

void client_pushback(Client* head, int fd){

	Client * c;
	

	while(head->next != NULL)
		head=head->next;
		
	if ((c = malloc(sizeof(Client))) == NULL ){
		perror("malloc: ");
		exit(-1);
	}
	
	c->fd=fd;
	c->next=NULL;
	head->next=c;
}


void client_delete(Client* head){
	
	Client* c;
	
	if(head == NULL)
		return;
	
	do{
		c=head;
		head=head->next;
		free(c);	
	}while (head->next!=NULL);
		free(head);
	
}


