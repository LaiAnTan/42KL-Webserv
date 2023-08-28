SOCKET = Socket.cpp

CLIENT = Client.cpp $(SOCKET)

SERVER = Server.cpp $(CLIENT)

WEBSERV = WebServer.cpp $(SERVER)

all: server client

server: $(WEBSERV) server_main.cpp
	g++ $(WEBSERV) server_main.cpp -o server

client: server client_main.cpp
	g++ $(SOCKET) client_main.cpp -o client

webserv:
	g++ *.cpp -o webserv
