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
	class Config;
}

namespace HDE
{
	class Server : public SimpleServer
	{
	private:
		int newsocket;
		string headers;
		string content;
		conf::Config *config;

		void accepter();
		void handler();
		void responder();

	public:
		Server(conf::Config *config);
		// ~Server();
		void launch();
		// static std::vector<string> get_bufferVEC();
		// static char	*get_bufferCHAR();
		string get_headers();
		string get_content();
		conf::Config *get_config();

		int		sendData(int sckt, const void *data, int datalen);
		void	dataSet(int socket);
		void	dataGet(int socket);
		void	icon(string type, int sock);
		void	png(string type, int sock);
		void	css(string type, int sock);
		void	html(string type, int sock);
		void	py(string type, int socket);
	};
}

#endif