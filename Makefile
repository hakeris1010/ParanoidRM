CXX 	 := g++
CXXFLAGS := -O2 -std=c++14
LDFLAGS  := -municode

PROGNAME := trashbin
PROGRAM  := bin/$(PROGNAME)
TESTGRAM := bin/test_$(PROGNAME)

PREFIX 	:= /usr/local/bin

SRC 	:= recycle-bin.cpp
TESTSRC := test.cpp

all: $(PROGRAM)

install: all
	@install $(PROGRAM) $(PREFIX)

tests: testops $(TESTGRAM)
testops:
	$(eval CXXFLAGS += -DTRASHBIN_TEST )

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

$(PROGRAM): $(SRC:.cpp=.o)
	$(CXX) $(LDFLAGS) -o $@ $^

$(TESTGRAM): $(TESTSRC:.cpp=.o) $(SRC:.cpp=.o)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) *.o


