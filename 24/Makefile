APP=test
CXXFLAGS=-lrt -pthread -lpthread
CC=g++
OBJ=test.o ../libs/MsgQueue/CVMsgQueue.o ../libs/Thread/Thread.o ../libs/Mutex/Mutex.o ../libs/CondVar/CondVar.o
M_HEADER = ../libs/Mutex/Mutex.h
T_HEADER = ../libs/Thread/Thread.h
MSQ_HEADER = ../libs/MsgQueue/MsgQueue.h
CVMSQ_HEADER = ../libs/MsgQueue/CVMsgQueue.h
CV_HEADER = ../libs/CondVar/CondVar.h

$(APP): $(OBJ)
	$(CC) -o $(APP) $(CXXFLAGS) $(OBJ)

../libs/Mutex/Mutex.o: ../libs/Mutex/Mutex.cpp $(M_HEADER)
	$(CC) -c ../libs/Mutex/Mutex.cpp -o ../libs/Mutex/Mutex.o

../libs/CondVar/CondVar.o: ../libs/CondVar/CondVar.cpp $(CV_HEADER)
	$(CC) -c ../libs/CondVar/CondVar.cpp -o ../libs/CondVar/CondVar.o

../libs/MsgQueue/MsgQueue.o: ../libs/MsgQueue/CVMsgQueue.cpp $(MSQ_HEADER) $(CVMSQ_HEADER)
	$(CC) -c  ../libs/MsgQueue/CVMsgQueue.cpp -o ../libs/MsgQueue/CVMsgQueue.o

../libs/Thread/Thread.o: ../libs/Thread/Thread.cpp $(T_HEADER)
	$(CC) -c ../libs/Thread/Thread.cpp -o ../libs/Thread/Thread.o

test.o: test.cpp 
	$(CC) -c test.cpp -o test.o

clean:
	rm -rf $(APP) $(OBJ) core


