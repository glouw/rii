SANITIZE = 0

CFLAGS = -Wall -Wextra -Wpedantic -g

ifeq (1, $(SANITIZE))
CFLAGS += -fsanitize=address -fsanitize=undefined
endif

BIN = rr

CC = gcc -std=c99

SRC = rr.c

INC = ctl/ctl

TESTS = tests
EXAMP = examples

define bad
	@echo "error?"
	./$(BIN) $(1); test "$$?" -eq 1
endef

examples: test
	@echo ""
	@echo "!! EXAMPLES"
	./$(BIN) $(EXAMP)/fact.rr
	@echo ">> PASS"

test: all
	@echo "!! TESTS"
	./$(BIN) $(TESTS)/for.rr
	./$(BIN) $(TESTS)/while.rr
	./$(BIN) $(TESTS)/entry.rr
	./$(BIN) $(TESTS)/ref.rr
	./$(BIN) $(TESTS)/fun.rr
	./$(BIN) $(TESTS)/if.rr
	./$(BIN) $(TESTS)/break.rr
	./$(BIN) $(TESTS)/continue.rr
	./$(BIN) $(TESTS)/arr.rr
	./$(BIN) $(TESTS)/2darr.rr
	./$(BIN) $(TESTS)/expr.rr
	./$(BIN) $(TESTS)/obj.rr
	./$(BIN) $(TESTS)/obj-no-find.rr
	./$(BIN) $(TESTS)/obj-arr-mix.rr
	./$(BIN) $(TESTS)/argv.rr testing cmd params
	$(call bad,tests/continue-bad.rr)
	$(call bad,tests/break-bad.rr)
	$(call bad,tests/args-bad.rr)
	$(call bad,tests/arr-oob-bad.rr)
	@echo ">> PASS"

all:
	$(CC) $(SRC) $(CFLAGS) -I $(INC) -o $(BIN)

clean:
	rm -f $(BIN)
