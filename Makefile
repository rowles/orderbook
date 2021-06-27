CXX = g++
CXXFLAGS = -Wall -Werror -Wfatal-errors -Wpedantic -Wextra -Wmissing-include-dirs -Wconversion -Wsign-conversion -std=c++2a -march=native -O3
INCS = -I./src/

SRC = $(wildcard src/*.hpp)
TEST_SRC = $(wildcard tests/*.cpp)

all:

.PHONY: test
test: 
	$(CXX) $(CXXFLAGS) $(INCS) $(TEST_SRC) -o test -lgtest -lpthread
	./test

