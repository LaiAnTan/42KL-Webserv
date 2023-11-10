#ifndef Server_HPP
#define Server_HPP

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
#include <sys/wait.h>

const int BUFFER_SIZE = 1024;

using std::string;

namespace conf
{
	class ServerConfig;
}

namespace HDE
{
	class Server
	{
		private:

			int							newsocket;
			string						headers;
			string						content;
			const conf::ServerConfig	*config;

			// function to send data (response) to client
			int		sendData(int sckt, const void *data, int datalen);

			// post
			void	handlePostRequest(int socket);
			void	handlePostResponse(int socket, string filename);

			// get
			void	handleGetRequest(int socket);
			void	handleGetResponse(string filename, int socket);

			// delete
			void	handleDeleteRequest(int socket);
			void	handleDeleteResponse(int socket, bool is_deleted);

			// helper
			void	sendError(string type, int socket);
			string	get_type(string extension);

			// for cgi
			void	py(string type, int socket);


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