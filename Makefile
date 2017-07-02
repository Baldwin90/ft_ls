FILES			= ls

DOTCFILES		= $(addprefix srcs/, $(addsuffix .c, $(FILES)))
OBJECT_FILES	= $(addprefix build/, $(addsuffix .o, $(FILES)))

NAME			= ft_ls

CC				= gcc -O3

CFLAGS			= -Wall -Werror -Wextra

INCLUDES		= -I ./srcs/libft/includes

LIBFT			= ./srcs/libft/libft.a

.PHONY: all clean fclean re

all: $(NAME)

clean:
		make -C ./srcs/libft/ clean
		rm -rf build/

fclean: clean
		make -C ./srcs/libft/ fclean
		rm -rf $(NAME)

re: fclean all
		make -C ./srcs/libft/ re

build:
		mkdir build/
		
build/%.o: srcs/%.c | build
		$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJECT_FILES)
		make -C ./srcs/libft/
		$(CC) $(OBJECT_FILES) $(LIBFT) -o $(NAME)
