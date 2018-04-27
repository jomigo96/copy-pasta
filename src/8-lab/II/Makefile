CC=gcc
CFLAGS=-Wall -pedantic -g -lpthread
DEPS = clipboard.h
OBJ = clipboard.o 
OBJ2 = library.o app_teste.o
OBJ3 = backup.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clipboard: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
		
app_teste: $(OBJ2)
	gcc -o $@ $^ $(CFLAGS)
	
backup: $(OBJ3)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm $(OBJ) $(OBJ2) $(OBJ3) clipboard app_teste backup
