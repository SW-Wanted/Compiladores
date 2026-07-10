CC        = cc
CFLAGS    = -Wall -Wextra -Werror -Iinclude -I../01-Lexer/include -I../02-Parser/include
NAME      = semantic
LIB       = libsemantic.a
LIBPARSER = ../02-Parser/libparser.a
LIBLEXER  = ../01-Lexer/liblexer.a
OBJDIR    = objs
SRCDIR    = src
TESTDIR   = tests
SRCS      = $(SRCDIR)/main.c $(SRCDIR)/semantic.c $(SRCDIR)/sem_decl.c \
            $(SRCDIR)/sem_stmt.c $(SRCDIR)/sem_expr.c $(SRCDIR)/sem_type_build.c \
            $(SRCDIR)/sem_types.c $(SRCDIR)/sem_symtab.c $(SRCDIR)/sem_error.c
OBJS      = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
MAIN_OBJ  = $(OBJDIR)/main.o
LIB_OBJS  = $(filter-out $(MAIN_OBJ),$(OBJS))

all: $(NAME)

$(LIBLEXER):
	$(MAKE) -C ../01-Lexer

$(LIBPARSER):
	$(MAKE) -C ../02-Parser

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(LIB_OBJS)
	ar rcs $@ $^

$(NAME): $(MAIN_OBJ) $(LIB) $(LIBPARSER) $(LIBLEXER)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) -L. -lsemantic \
		-L../02-Parser -lparser -L../01-Lexer -llexer

test: $(NAME)
	@for f in $(TESTDIR)/*.c; do \
		echo "==================== $$f ===================="; \
		./$(NAME) --all $$f; \
		echo ""; \
	done

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME) $(LIB)

re: fclean all

.PHONY: all test clean fclean re
