# === Configuration ===

CC 		:= 	gcc

CFLAGS	:=	-g3 -Wall -Wextra

NAME	:=	gameboy-emulator-c


SRC		:=	src/main.c	\
			src/cart.c	\

OBJ		:=	$(SRC:.c=.o)


# === Rules ===


all: 	$(NAME)

$(NAME):	$(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re:	fclean all

test:
	echo "No test."

.PHONY: all clean fclean re
