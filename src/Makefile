CC=gcc
CFLAGS=-Wall -pedantic 
LDFLAGS = -lpthread -lm
DEPS = clipboard.h list.h
OBJ= library.o app_teste.o clipboard.o  
EXECUTABLES = clipboard app_teste backup

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
all: $(OBJ)
	$(MAKE) clipboard
	$(MAKE) app_teste
	$(MAKE) backup

clipboard: clipboard.o 
	gcc -o $@ $^ $(LDFLAGS)
		
app_teste: app_teste.o library.o
	gcc -o $@ $^ $(LDFLAGS)
	

clean:
	rm -rf $(OBJ) $(EXECUTABLES) 
