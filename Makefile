CC       = gcc
CFLAGS   = -Wall -Wextra -Iinclude
BUILD    = build
OBJDIR   = $(BUILD)/obj

SRC_COMMON = src/lexer.c src/tokken.c src/utils.c src/lexer_regex.c src/ast.c src/parser.c
OBJ_COMMON = $(SRC_COMMON:src/%.c=$(OBJDIR)/%.o)

MAIN1 = src/main.c
BIN1  = $(BUILD)/parser

# MAIN2 = src/main_regex.c
# BIN2  = $(BUILD)/lexer_regex

.PHONY: all clean run1 debug format

all: $(BIN1)

$(BIN1): $(OBJ_COMMON) $(OBJDIR)/main.o
	$(CC) $(OBJ_COMMON) $(OBJDIR)/main.o -o $@

# $(BIN2): $(OBJ_COMMON) $(OBJDIR)/main_regex.o
#	$(CC) $(OBJ_COMMON) $(OBJDIR)/main_regex.o -o $@

# Compile .c → .o into build/obj/
$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run: $(BIN1)
	./$(BIN1)

# run2: $(BIN2)
#	./$(BIN2)

debug: CFLAGS += -g
debug: clean all

clean:
	rm -rf $(BUILD) *.o src/*.o

format:
	clang-format -i $(SRC_COMMON) $(MAIN1) include/*.h
