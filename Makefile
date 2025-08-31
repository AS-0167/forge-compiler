CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC_COMMON = src/lexer.c src/tokken.c src/utils.c src/lexer_regex.c
OBJ_COMMON = $(SRC_COMMON:.c=.o)

# Two different mains
MAIN1 = src/main.c
MAIN2 = src/main_regex.c

BIN1 = build/lexer
BIN2 = build/lexer_regex

all: $(BIN1) $(BIN2)

$(BIN1): $(OBJ_COMMON) $(MAIN1:.c=.o)
	mkdir -p build
	$(CC) $(OBJ_COMMON) $(MAIN1:.c=.o) -o $@

$(BIN2): $(OBJ_COMMON) $(MAIN2:.c=.o)
	mkdir -p build
	$(CC) $(OBJ_COMMON) $(MAIN2:.c=.o) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run1: $(BIN1)
	./$(BIN1)

run2: $(BIN2)
	./$(BIN2)

debug: CFLAGS += -g
debug: clean all

clean:
	rm -rf build *.o src/*.o

format:
	clang-format -i $(SRC_COMMON) $(MAIN1) $(MAIN2) include/*.h
