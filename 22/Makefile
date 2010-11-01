#/usr/bin/make -f

APP = bin
OBJ = source.o ../libs/Semaphore/Semaphore.o ../libs/Thread/Thread.o
CXXFLAGS = -lpthread -pthread
CC = g++

$(APP): $(OBJ)
	$(CC) $(CXXFLAGS) $(OBJ) -o $(APP)

../libs/Semaphore/Semaphore.o: ../libs/Semaphore/Semaphore.cpp ../libs/Semaphore/Semaphore.h
	g++ $(CXXFLAGS) -c -o ../libs/Semaphore/Semaphore.o ../libs/Semaphore/Semaphore.cpp

../libs/Thread/Thread.o: ../libs/Thread/Thread.cpp ../libs/Thread/Thread.h
	g++ $(CXXFLAGS) -c -o ../libs/Thread/Thread.o ../libs/Thread/Thread.cpp

source.o: source.cpp
	$(CC) -c source.cpp $(CXXFLAGS) -o source.o

clean:
	rm -rf core $(APP) $(OBJ)
