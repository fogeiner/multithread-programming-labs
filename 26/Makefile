APP = bin
OBJ = source.o terminal.o
CXXFLAGS = -lpthread
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp
	$(CC) -c -o source.o source.cpp $(CXXFLAGS)

terminal.o: terminal.h terminal.c
	$(CC) -o terminal.o -c terminal.c

clean:
	rm -rf $(APP) $(OBJ) core
