NAME = webserv

ODIR = obj

# ---SRCS---
SRCS_CDIR = srcs
SRCS_C = webserv.cpp Config.cpp ServerConfig.cpp ServerLocation.cpp WebSurf.cpp

# ---Network---
SERVER_CDIR = Networking/Server
SERVER_C = Get.cpp Post.cpp Server.cpp SimpleServer.cpp ServerListener.cpp

# ---Socket---
SOCKET_CDIR = Networking/Socket
SOCKET_C = SimpleSocket.cpp BindingSocket.cpp ListeningSocket.cpp ConnectingSocket.cpp

SRCS_O := $(addprefix $(ODIR)/,$(notdir $(SRCS_C:.cpp=.o)))
SERVER_O := $(addprefix $(ODIR)/,$(notdir $(SERVER_C:.cpp=.o)))
SOCKET_O := $(addprefix $(ODIR)/,$(notdir $(SOCKET_C:.cpp=.o)))

# imagine using CFLAGS
CFLAGS := -Wall -Wextra -Werror

vpath %.cpp $(SRCS_CDIR) $(SERVER_CDIR) $(SOCKET_CDIR)

all : $(NAME)

$(NAME) : $(SRCS_O) $(SERVER_O) $(SOCKET_O)
	g++ $(CFLAGS) $^ -o $@

$(ODIR) :
	@mkdir -p $@
	@mkdir -p Users

$(ODIR)/%.o: %.cpp | $(ODIR)
	g++ $(CFLAGS) -c $< -o $@;

clean :
	rm -f $(NAME)

fclean : clean
	rm -rf $(ODIR)
	rm -rf Users

re : fclean all

.PHONY: all clean fclean re