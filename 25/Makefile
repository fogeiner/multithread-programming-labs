#! /usr/bin/make -f

APP = retranslator
OBJ = retranslator.o Forwarded_connection/Forwarded_connection.o ../libs/Mutex/Mutex.o
CXXFLAGS = -pthread -lpthread
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

retranslator.o: Makefile retranslator.cpp ../libs/Fd_set/Fd_set.h ../libs/Buffer/Buffer.h ../libs/Buffer/Chunk/Chunk.h
	$(CC) -o retranslator.o -c $(CXXFLAGS) retranslator.cpp

Forwarded_connection/Forwarded_connection.o: Makefile Forwarded_connection/Forwarded_connection.cpp Forwarded_connection/Forwarded_connection.h
	$(CC) -o Forwarded_connection/Forwarded_connection.o -c $(CXXFLAGS) Forwarded_connection/Forwarded_connection.cpp

../libs/Mutex/Mutex.o: Makefile ../libs/Mutex/Mutex.cpp ../libs/Mutex/Mutex.h
	$(CC) -o ../libs/Mutex/Mutex.o ../libs/Mutex/Mutex.cpp -c $(CXXFLAGS)

clean:
	rm -rf core $(APP) $(OBJ)
