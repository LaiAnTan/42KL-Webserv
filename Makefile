NAME = webserv

ODIR = obj

# --main--
SRCS = srcs
SRCS_C =  WebSurf.cpp main.cpp

# ---config---
CONFIG_CDIR = srcs/config
CONFIG_C = Config.cpp ServerConfig.cpp ServerLocation.cpp

# --util--
UTIL_CDIR = srcs/util
UTIL_C = Utils.c

# ---Network---
SERVER_CDIR = srcs/server
SERVER_C = Delete.cpp Get.cpp Post.cpp Server.cpp SimpleServer.cpp ServerListener.cpp 

# ---Socket---
SOCKET_CDIR = srcs/server/socket
SOCKET_C = SimpleSocket.cpp BindingSocket.cpp ListeningSocket.cpp

SRCS_O := $(addprefix $(ODIR)/,$(notdir $(SRCS_C:.cpp=.o)))
CONFIG_O := $(addprefix $(ODIR)/,$(notdir $(CONFIG_C:.cpp=.o)))
UTIL_O := $(addprefix $(ODIR)/,$(notdir $(UTIL_C:.cpp=.o)))
SERVER_O := $(addprefix $(ODIR)/,$(notdir $(SERVER_C:.cpp=.o)))
SOCKET_O := $(addprefix $(ODIR)/,$(notdir $(SOCKET_C:.cpp=.o)))

# imagine using CFLAGS
CFLAGS := -Wall -Wextra -Werror -std=c++98

vpath %.cpp $(SRCS_CDIR) $(CONFIG_CDIR) $(UTIL_CDIR) $(SERVER_CDIR) $(SOCKET_CDIR)

all : $(NAME)

$(NAME) : $(SRCS_O) $(SERVER_O) $(SOCKET_O) $(UTIL_O) $(CONFIG_O)
	g++ $(CFLAGS) $^ -o $@

$(ODIR) :
	@mkdir -p $@
	@mkdir -p root

$(ODIR)/%.o: %.cpp | $(ODIR)
	g++ $(CFLAGS) -c $< -o $@;

clean :
	rm -f $(NAME)

fclean : clean
	rm -rf $(ODIR)
	rm -rf root

re : fclean all

.PHONY: all clean fclean re