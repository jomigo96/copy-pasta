#define POSITION_SIZE 128
#define MESSAGE_SIZE 128

#define SOCK_ADDRESS "./sock" 

#include <sys/types.h>

typedef struct message{
	
	short entry;
	char msg[MESSAGE_SIZE];
	short flag;
	
}Message;

typedef struct thread_parameters{

	int sock_id;
	char** repository;
	int mode;
	int sock_s_fd;
	
}T_param;


int clipboard_connect(char * clipboard_dir);
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count);
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count);
void clipboard_close(int clipboard_id);

