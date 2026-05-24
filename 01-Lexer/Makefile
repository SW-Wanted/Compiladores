NAME = liblexer.a
CC       = cc
CFLAGS   = -Wall -Wextra -Werror -Iincludes
SRC      = srcs/lexer.c srcs/token.c srcs/symbol_table.c
OBJ      = $(SRC:.c=.o)
TARGET   = lexer

all: $(TARGET)

$(NAME): $(OBJ)
	ar rcs $@ $^

$(TARGET): srcs/main.o $(NAME)
	$(CC) $(CFLAGS) -o $@ srcs/main.o -L. -llexer

srcs/%.o: srcs/%.c
	$(CC) $(CFLAGS) -c $< -o $@

lib: $(NAME)

test: $(TARGET)
	./$(TARGET) tests/teste.c

clean:
	rm -f $(TARGET) $(NAME) srcs/*.o

re: clean all

.PHONY: all lib test clean re
