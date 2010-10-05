APP = bin
FLAGS = -g -lpthread -pthread -std=c99 -Wall
OBJ = source.o
CC = mpicc

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(FLAGS)

source.o: source.c
	$(CC) -o source.o source.c $(FLAGS) -c

clean:
	rm -rf core $(APP) $(OBJ)
