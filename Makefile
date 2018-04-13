CC = gcc
CFLAGS= -O2 -std=c99
LDFLAGS= -municode

PROGNAME= trashbin
PROGRAM= bin/$(PROGNAME)

SRC= recycle-bin.c

all: $(PROGRAM)

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

$(PROGRAM): $(SRC:.c=.o)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) *.o

