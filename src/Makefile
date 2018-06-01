CC=gcc
CFLAGS= -Wall -pedantic -g
LDFLAGS = -lpthread -lm
DEPS = clipboard-dev.h
OBJ= app_teste.o clipboard.o clipboard_func.o app_teste2.o
EXECUTABLES = clipboard app_teste app_teste2
LPATH = $(shell pwd)/lib

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
%.so: library.c clipboard.h
	$(CC) $(CFLAGS) -fPIC -c library.c
	$(CC) -shared -Wl,-soname,libclipboard.so -o libclipboard.so library.o
	rm -rf library.o
	mv libclipboard.so $(LPATH)
	
all: $(OBJ)
	$(MAKE) clipboard
	$(MAKE) app_teste
	$(MAKE) app_teste2

clipboard: clipboard.o clipboard_func.o
	$(CC) -o $@ $^ $(LDFLAGS)
		
app_teste: app_teste.o ./lib/libclipboard.so
	$(CC) -o $@ app_teste.o $(LDFLAGS) -L$(LPATH) -lclipboard -Wl,-rpath=$(LPATH)
	
app_teste2: app_teste2.o ./lib/libclipboard.so
	$(CC) -o $@ app_teste2.o $(LDFLAGS) -L$(LPATH) -lclipboard -Wl,-rpath=$(LPATH)
	
clean:
	rm -rf $(OBJ) $(EXECUTABLES) ./lib/libclipboard.so
