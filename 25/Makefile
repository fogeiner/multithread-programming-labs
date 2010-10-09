#! /usr/bin/make -f

APP = retranslator
OBJ = retranslator.o Forwarded_connection/Forwarded_connection.o
CXXFLAGS =
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

retranslator.o: retranslator.cpp ../libs/Fd_set/Fd_set.h ../libs/Buffer/Buffer.h ../libs/Buffer/Chunk/Chunk.h
	$(CC) -o retranslator.o -c $(CXXFLAGS) retranslator.cpp

Forwarded_connection/Forwarded_connection.o: Forwarded_connection/Forwarded_connection.cpp Forwarded_connection/Forwarded_connection.h
	$(CC) -o Forwarded_connection/Forwarded_connection.o -c $(CXXFLAGS) Forwarded_connection/Forwarded_connection.cpp

clean:
	rm -rf core $(APP) $(OBJ)
