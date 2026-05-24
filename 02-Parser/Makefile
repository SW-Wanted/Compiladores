CC       = cc
CFLAGS   = -Wall -Wextra -Werror -Iincludes -I../01-Lexer/includes
NAME     = parser
LIB      = libparser.a
LIBLEXER = ../01-Lexer/liblexer.a
OBJDIR   = objs
SRCS     = srcs/main.c srcs/parser.c srcs/scope_table.c
OBJS     = $(SRCS:srcs/%.c=$(OBJDIR)/%.o)
MAIN_OBJ = $(OBJDIR)/main.o
LIB_OBJS = $(filter-out $(MAIN_OBJ),$(OBJS))
TEST_SRC = tests/test.c

all: $(NAME)

$(LIBLEXER):
	$(MAKE) -C ../01-Lexer

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: srcs/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(LIB_OBJS)
	ar rcs $@ $^

$(NAME): $(MAIN_OBJ) $(LIB) $(LIBLEXER)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) -L. -lparser -L../01-Lexer -llexer

test: $(NAME)
	./$(NAME) $(TEST_SRC)

clean:
	rm -rf $(OBJDIR)
	$(MAKE) -C ../01-Lexer clean

fclean: clean
	rm -f $(NAME) $(LIB)
	$(MAKE) -C ../01-Lexer fclean

re: fclean all

.PHONY: all test clean fclean re
