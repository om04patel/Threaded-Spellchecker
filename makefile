CC = gcc
CFLAGS = -Wall -g -std=c11
LIBS = -lpthread

all: spellcheck

spellcheck: spellcheck.o
	$(CC) $(CFLAGS) -o spellcheck spellcheck.o $(LIBS)
spellcheck.o: spellcheck.c
	$(CC) $(CFLAGS) -c spellcheck.c
clean:
	rm -f spellcheck *.o
