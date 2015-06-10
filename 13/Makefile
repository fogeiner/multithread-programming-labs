#/usr/bin/make -f

LDFLAGS= -pthread
CC= g++

all: semaphore mutex

mutex: mutex.o ../libs/Mutex/Mutex.o ../libs/Thread/Thread.o

semaphore: semaphore.o ../libs/Semaphore/Semaphore.o ../libs/Thread/Thread.o


clean:
	rm -rf core semaphore mutex *.o
