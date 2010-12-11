APP = bin
OBJ = source.o ../libs/Terminal/Terminal.o
CXXFLAGS =  -lrt  -g -lresolv -lnsl -lsocket
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp ../libs/ChunkBuffer/Buffer.h
	$(CC) -c -o source.o source.cpp $(CXXFLAGS)

../libs/Terminal/Terminal.o: ../libs/Terminal/Terminal.h ../libs/Terminal/Terminal.cpp
	$(CC) -o ../libs/Terminal/Terminal.o -c ../libs/Terminal/Terminal.cpp $(CXXFLAGS)

clean:
	rm -rf $(APP) $(OBJ) core
