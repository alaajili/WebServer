#
# Created by Abderrahmane Laajili on 1/30/23.
#
NAME = webserv
SRCS = webserv.cpp ParsingConfig/holder.cpp ParsingConfig/parser.cpp ParsingConfig/read_from_conf.cpp \
ParsingConfig/server_location.cpp Request/request.cpp
OBJS = $(SRCS:.cpp=.o)
CC = c++
CPPFLAGS = -Wall -Werror -Wextra -std=c++98
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all