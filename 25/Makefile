#! /usr/bin/make -f

LDFLAGS= -pthread
CC= g++

retranslator: retranslator.o Forwarded_connection/Forwarded_connection.o ../libs/Fd_set/Fd_set.h Buffer/Buffer.h Buffer/Chunk/Chunk.h

clean:
	rm -rf core retranslator *.o
