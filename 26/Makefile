#! /usr/bin/make -f
L = ../libs
S = TCPSocket
P = HTTPURIParser
T = Terminal
B = Buffer
VB = VectorBuffer
APP = bin
OBJ = source.o $(L)/$(T)/$(T).o $(L)/$(S)/$(S).o $(L)/$(P)/$(P).o
CXXFLAGS = -lpthread -g
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp $(L)/$(B)/$(VB).h
	$(CC) -c -o source.o source.cpp $(CXXFLAGS)

$(L)/$(T)/$(T).o: $(L)/$(T)/$(T).h $(L)/$(T)/$(T).cpp
	$(CC) -o $(L)/$(T)/$(T).o -c $(L)/$(T)/$(T).cpp $(CXXFLAGS)
$(L)/$(S)/$(S).o: $(L)/$(S)/$(S).h $(L)/$(S)/$(S).cpp
	$(CC) -o $(L)/$(S)/$(S).o -c $(L)/$(S)/$(S).cpp $(CXXFLAGS)
$(L)/$(P)/$(P).o: $(L)/$(P)/$(P).h $(L)/$(P)/$(P).cpp
	$(CC) -o $(L)/$(P)/$(P).o -c $(L)/$(P)/$(P).cpp $(CXXFLAGS)
clean:
	rm -rf $(APP) $(OBJ) core
