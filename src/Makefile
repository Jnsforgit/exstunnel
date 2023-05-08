# Makefile example to compile all .cpp files in current directory
# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -g -O2 -Wall -Wextra -Wpedantic -Wformat=2 -Wconversion -Wno-long-long
LDEXEFLAGS = -std=c++11 -mthreads -lws2_32
# Source files
SRC = $(wildcard *.cpp)
# Object files
OBJ = $(SRC:.cpp=.o)
# Output file
EXE = ../bin/exstunnel.exe

# Targets
all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $^ -o $@ $(LDEXEFLAGS) 
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@
clean:
	del /F /Q $(OBJ)