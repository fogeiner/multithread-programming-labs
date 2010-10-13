#/usr/bin/make -f

APP = bin
OBJ = source.o
FLAGS = -lpthread
CC = gcc

$(APP): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(APP)

source.o: source.c
	$(CC) -c source.c $(FLAGS) -o source.o

clean:
	rm -rf core $(APP) $(OBJ)
