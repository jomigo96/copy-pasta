#define MESSAGE_SIZE 256

#define SOCK_ADDRESS "./sock" 

#include <sys/types.h>

#define PEER_SERVICE 0
#define APP_SERVICE 1

#define DISCONNECT 0
#define COPY 1
#define PASTE 2
#define REDIRECT 3
#define WAIT 4
#define NOERROR 5

#define SIGALARM 14

typedef struct message{
	
	short entry; 
	char msg[MESSAGE_SIZE];
	size_t size; 
	short flag; 
	
}Message;

typedef struct clip_peers{
	
	int count;
	int * sock;
	int master;
}C_peers;

typedef struct thread_parameters{

	int sock_id;
	int mode;
	C_peers* peers;
	
}T_param;

struct clip_data{
	
	void* data[10];
	size_t size[10];
	short waiting[10];
	short writing[10];
};

/*!
 * \brief Application request handler
 * 
 * This thread is launched for every client that connects to the 
 * clipboad, be it another clipboard or an application. 
 * It services all requests until it 
 * disconnects, and then exits.
 * 
 * \param arg arg - pointer to the received data structure, of type 
 * T_param
 * 
 * \return This thread returns NULL upon exiting
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
 * \return This thread does not exit
 * */ 
void * thread_2_handler(void * arg);
 
/*!
 * \brief Global synchronization handler
 * 
 * This thread periodically sends the entire clipboard contents to the
 * connected peers.
 * 
 * \param arg arg - pointer to received data structure, of type C_peers.
 * 
 * \return This thread does not exit
 */
void * thread_3_handler(void * arg);

/*!
 * \brief Remove peer id from the peers vector
 * 
 * \param peerv Pointer to C_peers structure which stores all the peer 
 * ids 
 * 
 * \param fd socket id to be removed.
 * 
 * */
void remove_fd(C_peers * peerv, int fd);

void print_entry(int entry);

void ctrl_c_callback_handler(int signum);

void alarm_done(int signum);
