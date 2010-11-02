#/usr/bin/make -f

APP = bin
OBJ = source.o
FLAGS = -std=c99 -lpthread -pthread -lrt
CC = gcc

$(APP): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(APP)

source.o: source_independent.c
	$(CC) -c source_independent.c $(FLAGS) -o source.o

clean:
	rm -rf core $(APP) $(OBJ)
