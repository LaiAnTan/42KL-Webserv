#ifndef Server_HPP
#define Server_HPP

#include "SimpleServer.hpp"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fcntl.h>
#include "../../srcs/webserv.hpp"
#include <sys/wait.h>

const int BUFFER_SIZE = 1024;

namespace conf
{
	class ServerConfig;
}

namespace HDE
{
	class Server
	{
	private:
		int newsocket;

		string headers;
		string content;
		const conf::ServerConfig *config;

		// kenapalah mereka di public

		// tf is launch doing here?
		// void launch();

		// static std::vector<string> get_bufferVEC();
		// static char	*get_bufferCHAR();

		// post
		int		sendData(int sckt, const void *data, int datalen);
		void	dataSet(int socket);
		void	dataGet(int socket);

		// get
		// ...wtf
		void	icon(string type, int sock);
		void	png(string type, int sock);
		void	css(string type, int sock);
		void	html(string type, int sock);
		void	py(string type, int socket);

		// delete
		void	handleDeleteRequest();
		void	createDeleteResponse(int socket, string content, string content_type, bool is_deleted);

	public:
		Server(const conf::ServerConfig *config, int client_fd);
		~Server();

		int accepter();
		void handler();
		void responder();

		string						get_headers();
		string						get_content();
		const conf::ServerConfig	*get_config();
		int							get_socket();
	};
}

#endif