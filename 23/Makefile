#!/usr/bin/make -f

LDFLAGS= -pthread
LDLIBS= -lrt
CC=g++

main: main.o ../libs/Semaphore/Semaphore.o ../libs/MsgQueue/SemMsgQueue.o ../libs/Thread/Thread.o

clean:
	rm -rf core main *.o


