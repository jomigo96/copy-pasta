#include "clipboard.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int clipboard_connect(char * clipboard_dir){
	char fifo_name[100];
	
	sprintf(fifo_name, "%s%s", clipboard_dir, INBOUND_FIFO);
	int fifo_send = open(fifo_name, O_WRONLY);
	sprintf(fifo_name, "%s%s", clipboard_dir, OUTBOUND_FIFO);
	int fifo_recv = open(fifo_name, O_RDONLY);
	if(fifo_recv < 0)
		return fifo_recv;
	else
		return fifo_send;
}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	
	Message m;

	if ((count-1) > sizeof(char)*MESSAGE_SIZE)
		return -1;
	if ((region>9)||(region<0))
		return -1;
	
	m.entry = region;
	m.flag = 1;
	
	
	strcpy(m.msg, (char*)buf);
	

	printf("%s\n",m.msg);
		
	write(clipboard_id, &m, sizeof(Message));
	read(clipboard_id+1, &m, sizeof(Message));
	
	if (m.flag == 1)
		return 1;
	else
		return -1;
	
}
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	
	Message m;
	int m_size;	
	
		
	if ((region>9)||(region<0))
		return -1;
		
	m.entry = region;
	m.flag = 2;
		
	write(clipboard_id, &m, sizeof(Message));
	read(clipboard_id+1, &m, sizeof(Message));
	
	if (m.flag == 2){
		m_size=strlen(m.msg)+1;
		printf("%s\n",m.msg);
			
		strcpy((char*)buf, m.msg);
		return m_size;
		
	}
	else
		return -1;
	
}
