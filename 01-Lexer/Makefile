CC       = cc
CFLAGS   = -Wall -Wextra -Werror -Iincludes
NAME     = lexer
LIB      = liblexer.a
OBJDIR   = objs
LIBSRCS  = srcs/lexer.c srcs/token.c srcs/symbol_table.c
LIBOBJS  = $(LIBSRCS:srcs/%.c=$(OBJDIR)/%.o)
MAIN_OBJ = $(OBJDIR)/main.o
TEST_SRC = tests/teste.c

all: $(NAME)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: srcs/%.c | $(OBJDIR)
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
