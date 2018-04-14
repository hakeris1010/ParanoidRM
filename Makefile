CXX := g++
CXXFLAGS := -O2 -std=c++11
LDFLAGS := -municode

PROGNAME := trashbin
PROGRAM := bin/$(PROGNAME)

SRC := recycle-bin.cpp
TESTSRC := test.cpp

all: release
release: $(PROGRAM)

test: testops $(PROGRAM)
testops: 
	$(eval SRC += $(TESTSRC) )
	$(eval CXXFLAGS += -DTRASHBIN_TEST )

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

$(PROGRAM): $(SRC:.cpp=.o)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) *.o

