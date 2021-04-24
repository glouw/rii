CFLAGS = -Wall -Wextra -Wpedantic -g -fsanitize=address -fsanitize=undefined 

BIN = rii

SRC = rii.c

all: 
	gcc $(SRC) -o $(BIN) $(CFLAGS) -I ctl/ctl
