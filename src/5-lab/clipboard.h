#define INBOUND_FIFO "INBOUND_FIFO"
#define OUTBOUND_FIFO "OUTBOUND_FIFO"
#define POSITION_SIZE 128
#define MESSAGE_SIZE 128
#include <sys/types.h>

typedef struct message{
	
	short entry;
	char msg[MESSAGE_SIZE];
	short flag;
	
}Message;


int clipboard_connect(char * clipboard_dir);
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count);
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count);

