CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
BIN=AgglomerativeClustering

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) $^
	rm -f *.o

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)
