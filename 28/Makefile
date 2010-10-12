APP = bin
OBJ = source.o ../libs/Terminal/terminal.o
CXXFLAGS = -lpthread
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp
	$(CC) -c -o source.o source.cpp $(CXXFLAGS)

../libs/Terminal/terminal.o: ../libs/Terminal/terminal.h ../libs/Terminal/terminal.cpp
	$(CC) -o ../libs/Terminal/terminal.o -c ../libs/Terminal/terminal.cpp

clean:
	rm -rf $(APP) $(OBJ) core
