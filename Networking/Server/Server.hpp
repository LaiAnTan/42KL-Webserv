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
		static string headers;
		static string content;

		void accepter();
		void handler();
		void responder();
		conf::Config *config;

	public:
		Server(conf::Config *config);
		// ~Server();
		void launch();
		// static std::vector<string> get_bufferVEC();
		// static char	*get_bufferCHAR();
		static string get_headers();
		static string get_content();
	};
}

int sendData(int sckt, const void *data, int datalen);
void dataSet(int socket);
void dataGet(int socket);

#endif