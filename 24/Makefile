#!/usr/bin/make -f

LDFLAGS= -pthread
LDLIBS= -lrt
CC=g++

main: main.o ../libs/MsgQueue/CVMsgQueue.o ../libs/Thread/Thread.o ../libs/Mutex/Mutex.o ../libs/CondVar/CondVar.o

clean:
	rm -rf core main *.o


