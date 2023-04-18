# 定义编译器
CC=gcc
# 定义编译选项
CFLAGS=-Wall -Wextra -O2
# 定义目标文件
TARGET=exstunnel.exe
# 查找并获取所有的 .c 文件
SRCS=$(wildcard *.c)
# 将所有的 .c 文件转化为 .o 文件
OBJS=$(SRCS:.c=.o)

# 定义默认目标
all: $(TARGET)
# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) -o $@ $^
# 依次生成所有的 .o 文件
%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<
# 清除编译生成的文件
clean:
	rm -f $(TARGET) $(OBJS)