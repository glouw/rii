CFLAGS = -Wall -Wextra -Wpedantic -g #-fsanitize=address -fsanitize=undefined 

BIN = rr

SRC = rr.c

CC = gcc -std=c99

all: 
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) -I ctl/ctl
