
CXX = g++
CXXFLAGS = -Wall -Werror -Wfatal-errors -Wpedantic -Wextra -Wmissing-include-dirs -Wconversion -Wsign-conversion -std=c++2a -O0
CXXFLAGS = -Wall -Werror -Wfatal-errors -Wpedantic -Wextra -Wmissing-include-dirs -Wconversion -Wsign-conversion -std=c++2a -march=native -O3
INCS = -I./src

SRC = $(wildcard src/*.cpp)
TEST_SRC = $(wildcard tests/*.cpp)

all:

.PHONY: test
test:
	$(CXX) $(CXXFLAGS) $(INCS) -o test -lgtest $(TEST_SRC)
	./test

build:
	$(CXX) $(CXXFLAGS) $(INCS) -o run $(SRC)
	./run


