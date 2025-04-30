# === Configuration ===

CC 		:= 	gcc
NAME	:=	gameboy-emulator-c

CFLAGS	:=	-Wall -Wextra -DNAME_EXEC=\"$(NAME)\" -Iinclude
DFLAGS	:=	-g3 -DDEBUG

SRC		:=	src/main.c	\
			src/parser_args.c	\
			src/CPU/cpu.c	\
			src/CPU/cpu_utils.c	\
			src/CPU/cpu_proc.c	\
			src/CPU/cpu_fetch_data.c	\
			src/CPU/cpu_ie.c	\
			src/bus.c	\
			src/ram.c	\
			src/cart.c	\
			src/instructions.c	\

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
