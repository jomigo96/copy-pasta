CC=gcc
CPPFLAGS=-c -Wall -pedantic
LDFLAGS=-lm
EXECUTABLE=main
DOCC=doxygen

SOURCES=$(wildcard *.c)
OBJECTS=$(notdir $(SOURCES:.c=.o))
DOCDIR=../doc

all: $(SOURCES) $(EXECUTABLE) doc
  
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@ 

.cc.o:
	$(CC) $(CPPFLAGS) $< -o $@

doc:
	@$(DOCC) 1>/dev/null

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) $(DOCDIR)/html $(DOCDIR)/latex

run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: all doc clean
