APP = bin
OBJ = source.o
CXXFLAGS = -lpthread
CC = g++

$(APP): $(OBJ)
	$(CC) -o $(APP) $(OBJ) $(CXXFLAGS)

source.o: source.cpp
	$(CC) -c -o source.o source.cpp $(CXXFLAGS)

clean:
	rm -rf $(APP) $(OBJ) core
