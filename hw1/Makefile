CC = gcc
CFLAGS = -Wall -g -Iinclude

INC_DIR = include
SRC_DIR = src

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)
EXEC = cs5374_sh

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm $(SRC_DIR)/*.o $(EXEC)