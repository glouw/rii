CFLAGS = -Wall -Wextra -Wpedantic -g

BIN = rr

CC = gcc -std=c99

all:
	$(CC) main.c $(CFLAGS) -I ctl/ctl -o $(BIN) && ./$(BIN)

clean:
	rm -f $(BIN)
