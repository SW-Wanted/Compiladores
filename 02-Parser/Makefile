CC       = cc
CFLAGS   = -Wall -Wextra -Werror -Iinclude -I../01-Lexer/include
NAME     = parser
LIB      = libparser.a
LIBLEXER = ../01-Lexer/liblexer.a
OBJDIR   = objs
SRCDIR   = src
TESTDIR  = tests
SRCS     = $(SRCDIR)/main.c $(SRCDIR)/parser.c $(SRCDIR)/parser_declarations.c $(SRCDIR)/parser_statements.c $(SRCDIR)/parser_expressions.c $(SRCDIR)/parser_symbols.c $(SRCDIR)/parser_utils.c $(SRCDIR)/parser_error.c $(SRCDIR)/ast.c $(SRCDIR)/scope_table.c
TEST_SRC = $(TESTDIR)/test.c
OBJS     = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
MAIN_OBJ = $(OBJDIR)/main.o
LIB_OBJS = $(filter-out $(MAIN_OBJ),$(OBJS))

all: $(NAME)

$(LIBLEXER):
	$(MAKE) -C ../01-Lexer

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
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
