CC      = gcc
CFLAGS  = -Wall -Wextra -Iinclude
SRC     = src/main.c src/lexer.c src/token.c src/symbol_table.c
TARGET  = lexer

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGET)
	./$(TARGET) tests/teste.c

clean:
	rm -f $(TARGET)
