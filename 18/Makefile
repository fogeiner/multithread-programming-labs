#/usr/bin/make -f

APP = bin
OBJ = source.o ../libs/Mutex/Mutex.o
FLAGS = -lpthread
CC = g++

$(APP): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(APP)

../libs/Mutex/Mutex.o: ../libs/Mutex/Mutex.cpp ../libs/Mutex/Mutex.h
	$(CC) -c -o ../libs/Mutex/Mutex.o $(FLAGS) ../libs/Mutex/Mutex.cpp

source.o: source.cpp
	$(CC) -c source.cpp $(FLAGS) -o source.o

clean:
	rm -rf core $(APP) $(OBJ)
