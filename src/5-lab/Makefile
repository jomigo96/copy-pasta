CC=gcc
CFLAGS=-Wall -pedantic
DEPS = clipboard.h
OBJ = clipboard.o 
OBJ2 = library.o app_teste.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clipboard: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
	
app_teste: $(OBJ2)
	gcc -o $@ $^ $(CFLAGS)
