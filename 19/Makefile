#/usr/bin/make -f

APP = bin
OBJ = source.o
FLAGS = -lpthread
CC = g++

$(APP): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(APP)

source.o: source.cpp
	$(CC) -c source.cpp $(FLAGS) -o source.o

clean:
	rm -rf core $(APP) $(OBJ)
