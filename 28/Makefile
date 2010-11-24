APP = bin
OBJ = source.o ../libs/Terminal/terminal.o ../libs/Mutex/Mutex.o
T_HEADER = ../libs/Terminal/terminal.h
B_HEADER = ../libs/ChunkBuffer/Buffer.h
M_HEADER = ../libs/Mutex/Mutex.h
CXXFLAGS = -lpthread
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp $(B_HEADER)
	$(CC) -c -o source.o source.cpp $(CXXFLAGS)

../libs/Terminal/terminal.o: $(T_HEADER) ../libs/Terminal/terminal.cpp
	$(CC) -o ../libs/Terminal/terminal.o -c ../libs/Terminal/terminal.cpp $(CXXFLAGS)

../libs/Mutex/Mutex.o: $(M_HEADER) ../libs/Mutex/Mutex.cpp
	$(CC) -o ../libs/Mutex/Mutex.o -c ../libs/Mutex/Mutex.cpp $(CXXFLAGS)
clean:
	rm -rf $(APP) $(OBJ) core
