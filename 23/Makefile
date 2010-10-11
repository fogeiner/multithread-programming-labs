APP=test
CXXFLAGS=-lrt -pthread -lpthread
CC=g++
OBJ=test.o ../libs/Semaphore/Semaphore.o ../libs/MsgQueue/MsgQueue.o ../libs/Thread/Thread.o

$(APP): $(OBJ)
	$(CC) -o $(APP) $(CXXFLAGS) $(OBJ)

../libs/Semaphore/Semaphore.o: ../libs/Semaphore/Semaphore.cpp ../libs/Semaphore/Semaphore.h
	$(CC) -c  ../libs/Semaphore/Semaphore.cpp -o ../libs/Semaphore/Semaphore.o

../libs/MsgQueue/MsgQueue.o: ../libs/MsgQueue/MsgQueue.cpp ../libs/MsgQueue/MsgQueue.h
	$(CC) -c  ../libs/MsgQueue/MsgQueue.cpp -o ../libs/MsgQueue/MsgQueue.o

../libs/Thread/Thread.o: ../libs/Thread/Thread.cpp ../libs/Thread/Thread.h
	$(CC) -c ../libs/Thread/Thread.cpp -o ../libs/Thread/Thread.o

test.o: test.cpp 
	$(CC) -c test.cpp -o test.o

clean:
	rm -rf $(APP) $(OBJ) core


