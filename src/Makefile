# Makefile example to compile all .cpp files in current directory
# Compiler
CXX=g++
# Compiler flags
CXXFLAGS=-g -Wall
# Source files
SRC=$(wildcard *.cpp)
# Object files
OBJ=$(SRC:.cpp=.o)
# Output file
EXE=../bin/exstunnel
# Targets
all: $(EXE)
$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@
clean:
	del /F $(OBJ) $(EXE)