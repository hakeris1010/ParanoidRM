CXX = g++
CXXFLAGS= -O2 -std=c99
LDFLAGS= -municode

PROGNAME= trashbin
PROGRAM= bin/$(PROGNAME)

SRC= recycle-bin.cpp

all: $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $*.c -o $*.o

$(PROGRAM): $(SRC:.c=.o)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) *.o

