typedef struct client{
	
	int fd;
	struct client* next;
	
}Client;

Client* client_init(void);
Client* client_remove(Client* head, int fd);
void client_pushback(Client* head, int fd);
void client_delete(Client* head);
