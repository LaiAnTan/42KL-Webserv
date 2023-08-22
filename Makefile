NAME = webserv

CDIR = srcs

ODIR = obj

SRCS_C = webserv.cpp Config.cpp ServerConfig.cpp

SRCS_O := $(addprefix $(ODIR)/,$(notdir $(SRCS_C:.cpp=.o)))

# CFLAGS := -Wall -Wextra -Werror

vpath %.cpp srcs

all : $(NAME)

$(NAME) : $(SRCS_O)
	g++ $(CFLAGS) $(SRCS_O) -o $@

$(ODIR) :
	@mkdir -p $@

$(ODIR)/%.o: %.cpp | $(ODIR)
	@gcc -std=c++98 $(CFLAGS) $(LIBS_O) -c $< -o $@;

clean :
	rm -f $(NAME)

fclean :
	rm -f $(NAME)
	rm -rf $(ODIR)

re : fclean all

.PHONY: all clean fclean re