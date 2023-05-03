# Function to recursively search directory and sub-directory for target files matching the pattern
rwildcard	= $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

CC 			= gcc
CXX 		= g++
CFLAGS 		= -Wall -O2 -g
CXXFLAGS 	= -Wall -O2 -std=c++11 -g
LDFLAGS 	=
INC_DIR 	= -I./src/easylogger/inc
OBJDIR 		= ./bin
SRCS 		:= $(call rwildcard,src,*.c)
SRCS 		+= $(call rwildcard,src,*.cpp)
OBJS 		:= $(SRCS:.c=.o)
OBJS		:= $(OBJS:.cpp=.o)
AFILE		= core easylogger
AOBJS		:= $(SRCS:.c=.o)
TARGET 		:= app.exe

.PHONY: all
all: $(TARGET)
$(TARGET): $(OBJDIR)/%.a
	$(CXX) $(LDFLAGS) $^ -o $(OBJDIR)/$@

AFILE : $(OBJDIR)/$(OBJS)

%.o : %.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $(OBJDIR)/$@
%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $(OBJDIR)/$@

.PHONY: clean
clean:
	$(RM) $(OBJDIR)/$(OBJS) $(OBJDIR)/$(TARGET)

