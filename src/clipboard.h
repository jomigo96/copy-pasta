#define MESSAGE_SIZE 128

#define SOCK_ADDRESS "./sock" 

#include <sys/types.h>

#define PEER_SERVICE 0
#define APP_SERVICE 1

#define DISCONNECT 0
#define COPY 1
#define PASTE 2
#define REDIRECT 3
#define NOERROR 4

typedef struct message{
	
	short entry; 
	char msg[MESSAGE_SIZE];
	size_t size; 
	short flag; 
	
}Message;

typedef struct clip_peers{
	
	int count;
	int * sock;
}C_peers;

typedef struct thread_parameters{

	int sock_id;
	int mode;
	C_peers* peers;
	
}T_param;

struct clip_data{
	
	void* data[10];
	size_t size[10];
};


int clipboard_connect(char * clipboard_dir);
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count);
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count);
void clipboard_close(int clipboard_id);



/*!
 * \brief Application request handler
 * 
 * This thread is launched for every application that connects to the 
 * clipboad. It services the application's requests until it 
 * disconnects, and then exits.
 * 
 * \param arg arg - pointer to the received data structure, of type 
 * T_param
 * 
 * \return This thread does not returns NULL upon exiting
 * */
void * thread_1_handler(void * arg);

/*!
 * \brief Remote connection handler
 * 
 * This thread accepts connections from other clipboards and stores the
 * descriptor id's to be used.
 * 
 * \param arg arg - pointer to received data structure, of type C_peers.
 * 
 * \return This thread does not returns NULL upon exiting
 * */ 
void * thread_2_handler(void * arg);


/*!
 * \brief Remove peer id from the peer vector
 * 
 * \param peerv Pointer to C_peers structure which stores all the peer 
 * ids 
 * 
 * \param fd socket id to be removed.
 * 
 * */
void remove_fd(C_peers * peerv, int fd);

void print_entry(int entry);

