CC       = cc
CFLAGS   = -Wall -Wextra -Werror -Iinclude
NAME     = lexer
LIB      = liblexer.a
OBJDIR   = objs
SRCDIR	 = src
TESTDIR	 = tests
LIBSRCS  = $(SRCDIR)/lexer.c $(SRCDIR)/token.c $(SRCDIR)/symbol_table.c
TEST_SRC = $(TESTDIR)/teste.c
LIBOBJS  = $(LIBSRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
MAIN_OBJ = $(OBJDIR)/main.o

all: $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(LIBOBJS)
	ar rcs $@ $^

$(NAME): $(MAIN_OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) -L. -llexer

lib: $(LIB)

test: $(NAME)
	./$(NAME) $(TEST_SRC)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME) $(LIB)

re: fclean all

.PHONY: all lib test clean fclean re
