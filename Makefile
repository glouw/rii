CFLAGS = -Wall -Wextra -Wpedantic -g
#CFLAGS+= -fsanitize=address -fsanitize=undefined

BIN = rr

CC = gcc -std=c99

SRC = rr.c

INC = ctl/ctl

all:
	$(CC) $(SRC) $(CFLAGS) -I $(INC) -o $(BIN)

test: all
	./$(BIN) tests/for.rr
	./$(BIN) tests/while.rr
	./$(BIN) tests/entry.rr
	./$(BIN) tests/ref.rr
	./$(BIN) tests/fun.rr
	./$(BIN) tests/if.rr
	./$(BIN) tests/break.rr
	./$(BIN) tests/continue.rr
	./$(BIN) tests/argv.rr testing cmd params
	./$(BIN) examples/fact.rr
	@printf "\n>> ALLS WELL THAT ENDS WELL\n\n"

clean:
	rm -f $(BIN)
