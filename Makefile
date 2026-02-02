NAME= 			run
BONUS=			checker
CFLAGS= 		-Wall -Werror -Wextra -Ilibft
CC= 			cc
RM= 			rm -rf

SRCS=			coders/main.c \
				coders/gc.c \
				coders/parsing.c \
				coders/threads.c \
				coders/coders.c \
				coders/monitor.c \
				coders/tools.c

OBJS=			$(SRCS:.c=.o)

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CC) $(OBJS) -o $(NAME)
%.o:%.c
				$(CC) $(CFLAGS) -c $< -o $@

clean :
				$(RM) $(OBJS)

fclean : 		clean
				$(RM) $(NAME)

re: 			fclean all

.PHONY: 		all clean fclean re
