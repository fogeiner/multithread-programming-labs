APP=bin
CXXFLAGS=-lrt -pthread -lpthread -g
CC=g++
OBJ=source.o Semaphore.o MsgQueue.o Thread.o

$(APP): $(OBJ)
	$(CC) -o $(APP) $(CXXFLAGS) $(OBJ)

Semaphore.o: Semaphore.cpp Semaphore.h
	$(CC) -c  Semaphore.cpp

MsgQueue.o: MsgQueue.cpp MsgQueue.h
	$(CC) -c  MsgQueue.cpp

Thread.o: Thread.cpp Thread.h
	$(CC) -c Thread.cpp

source.o: source.cpp 
	$(CC) -c source.cpp

clean:
	rm -rf $(APP) $(OBJ) core
