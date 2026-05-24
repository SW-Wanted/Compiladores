CC       = cc
CFLAGS   = -Wall -Wextra -Werror -Iincludes -I../01-Lexer/includes
SRC      = srcs/main.c srcs/parser.c srcs/scope_table.c
OBJ      = $(SRC:.c=.o)
TARGET   = parser
LIBPARSER= libparser.a
LIBLEXER = ../01-Lexer/liblexer.a

all: $(TARGET)

$(LIBLEXER):
	cd ../01-Lexer && make

$(LIBPARSER): srcs/parser.o srcs/scope_table.o
	ar rcs $@ $^

srcs/%.o: srcs/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): srcs/main.o $(LIBPARSER) $(LIBLEXER)
	$(CC) $(CFLAGS) -o $@ srcs/main.o -L. -lparser -L../01-Lexer -llexer

clean:
	rm -f $(TARGET) $(LIBPARSER) srcs/*.o

re: clean all

.PHONY: all clean re
