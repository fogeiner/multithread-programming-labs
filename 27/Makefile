APP = bin
OBJ = source.o ../libs/Terminal/terminal.o
CXXFLAGS =  -lrt  -g
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp ../libs/ChunkBuffer/Buffer.h
	$(CC) -c -o source.o source.cpp $(CXXFLAGS)

../libs/Terminal/terminal.o: ../libs/Terminal/terminal.h ../libs/Terminal/terminal.cpp
	$(CC) -o ../libs/Terminal/terminal.o -c ../libs/Terminal/terminal.cpp $(CXXFLAGS)

clean:
	rm -rf $(APP) $(OBJ) core
