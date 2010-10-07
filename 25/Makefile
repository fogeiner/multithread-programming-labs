#! /usr/bin/make -f

APP = bin
OBJ = source.o
CXXFLAGS = 
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp
	$(CC) -c -o source.o $(CXXFLAGS) source.cpp

clean:
	rm -rf core $(APP) $(OBJ)
