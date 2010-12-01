#! /usr/bin/make -f

L = ../libs
A = $(L)/AsyncDispatcher/AsyncDispatcher
Lo = $(L)/Logger/Logger
P = proxy
S = $(L)/TCPSocket/TCPSocket

APP = proxy
OBJ =  $(A).o $(Lo).o $(P).o $(S).o
CXXFLAGS = -g -Wall -pedantic
CC = g++

$(APP): $(OBJ) 

$(A).o: $(A).cpp $(A).h

$(Lo).o: $(Lo).cpp $(Lo).h

$(P).o: $(P).cpp config.h

$(S).o: $(S).cpp $(S).h

clean:
	rm -rf $(APP) $(OBJ) core
