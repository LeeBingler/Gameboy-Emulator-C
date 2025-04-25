# === Configuration ===

CC 		:= 	gcc
NAME	:=	gameboy-emulator-c

CFLAGS	:=	-Wall -Wextra -DNAME_EXEC=\"$(NAME)\" -Iinclude
DFLAGS	:=	-g3 -DDEBUG

SRC		:=	src/main.c	\
			src/cart.c	\

OBJ		:=	$(SRC:.c=.o)


# === Rules ===


all: 	$(NAME)


debug: 	CFLAGS += $(DFLAGS)
debug: 	$(NAME)

$(NAME):	$(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re debug
