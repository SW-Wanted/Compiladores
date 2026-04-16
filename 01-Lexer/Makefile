CC      = cc
CFLAGS  = -Wall -Wextra -Werror -Iincludes
SRC     = srcs/main.c srcs/lexer.c srcs/token.c srcs/symbol_table.c
TARGET  = lexer

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGET)
	./$(TARGET) tests/teste.c

clean:
	rm -f $(TARGET)

re: clean all

.PHONY: all test clean re
