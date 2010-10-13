#/usr/bin/make -f

APPS = semaphore mutex
OBJ1 = ../libs/Semaphore/Semaphore.o ../libs/Thread/Thread.o semaphore.o
OBJ2 =  ../libs/Mutex/Mutex.o ../libs/Thread/Thread.o mutex_analogue.o
CXXFLAGS = -lpthread -pthread 
CC = g++

all: $(APPS)

semaphore: $(OBJ1) 
	$(CC) $(CXXFLAGS) $(OBJ1) -o semaphore

mutex: $(OBJ2) 
	$(CC) $(CXXFLAGS) $(OBJ2) -o mutex 

mutex_analogue.o: mutex_analogue.cpp
	$(CC) $(CXXFLAGS) -c mutex_analogue.cpp -o mutex_analogue.o

semaphore.o: semaphore.cpp
	$(CC) $(CXXFLAGS) -c semaphore.cpp -o semaphore.o

../libs/Semaphore/Semaphore.o: ../libs/Semaphore/Semaphore.cpp ../libs/Semaphore/Semaphore.h
	$(CC) $(CXXFLAGS) -c -o ../libs/Semaphore/Semaphore.o ../libs/Semaphore/Semaphore.cpp

../libs/Thread/Thread.o: ../libs/Thread/Thread.cpp ../libs/Thread/Thread.h
	$(CC) $(CXXFLAGS) -c -o ../libs/Thread/Thread.o ../libs/Thread/Thread.cpp

../libs/Mutex/Mutex.o: ../libs/Mutex/Mutex.cpp ../libs/Mutex/Mutex.h
	$(CC) $(CXXFLAGS) -c -o ../libs/Mutex/Mutex.o ../libs/Mutex/Mutex.cpp

clean:
	rm -rf core $(APPS) $(OBJ1) $(OBJ2) 
