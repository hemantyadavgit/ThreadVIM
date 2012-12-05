CC=gcc
cflags= -Wall -g -D_REENTRANT -lpthread -lcurses

all: editor

editor: textbuff.o editor.c
	$(CC) editor.c textbuff.o  $(cflags) -o editor
textbuff.o: textbuff.c textbuff.h
	$(CC) -c textbuff.c  -o textbuff.o $(cflags)
clean:
	rm -f *.o *.gch