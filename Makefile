
CXX = g++
CXXFLAGS = -std=c++2a -Wall
INCS = -I./src

TEST_SRC = $(wildcard tests/*.cpp)

all:

test:
	$(CXX) $(CXXFLAGS) $(INCS) -o test -lgtest $(TEST_SRC)

build:

