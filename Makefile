# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/14 13:32:51 by dclark            #+#    #+#              #
#    Updated: 2022/09/27 16:54:16 by asaboure         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

IDIR	=	./includes
CC		=	c++
CFLAGS	=	-Wall -Werror -Wextra -std=c++98 -I$(IDIR) -g

ODIR	=	./.obj
SDIR	=	./src

NAME	=	webserv

_DEPS	=	Create_socket.hpp \
			ft_itoa_string.hpp \
			request_client.hpp \
			Server.hpp \
			cgi_handler.hpp \
			Request.hpp \
			utils.hpp 

DEPS	=	$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ	=	main.o \
			Create_socket.o \
			ft_itoa_string.o \
			request_client.o \
			Server.o \
			cgi_handler.o \
			Request.o \
			utils.o

OBJ		=	$(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:	$(NAME)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(ODIR)/*.o

fclean:	clean
	rm -f $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
