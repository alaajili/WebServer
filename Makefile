#
# Created by Abderrahmane Laajili on 1/30/23.
#
NAME = webserv
SRCS = webserv.cpp ParsingConfig/holder.cpp ParsingConfig/parser.cpp ParsingConfig/read_from_conf.cpp Response/GetReq.cpp \
ParsingConfig/server_location.cpp Request/request.cpp Request/request_parser.cpp Request/utils.cpp
OBJS = $(SRCS:.cpp=.o)
CC = c++
CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g3
all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all