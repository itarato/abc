CXX=clang++
CXXFLAGS=`sdl2-config --cflags` -g -std=c++2a -Wall -pedantic
BIN=main

SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) `sdl2-config --libs` -lSDL2_ttf -lSDL2_image $^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm -f ./$(BIN)
