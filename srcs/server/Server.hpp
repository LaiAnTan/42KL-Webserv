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

	enum ServerStatus {
		NEW,
		READING_DATA,
		SENDING_DATA,
		DONE
	};

	class Server
	{
		private:

			int							newsocket;
			int							content_length;
			string						headers;
			string						content;
			string						curr_post_file_path;
			const conf::ServerConfig	*config;

			ServerStatus				status;
			std::ifstream				file;
			std::stringstream			chunk_to_send;

			// function to send data (response) to client
			int		sendData(int sckt, const void *data, int datalen);

			// post
			int	handlePostRequest();
			int	handlePostResponse();

			// get
			int	handleGetRequest();
			int	handleGetResponse(string filename);

			// delete
			int	handleDeleteRequest();
			int	handleDeleteResponse(bool is_deleted);

			// helper
			int		sendError(string type);
			string	get_type(string extension);
			bool	file_exists(std::string path);
			string	get_file_data(std::string filename);

			int		readOnce();

			// for cgi
			int	py();

			int	send_next_chunk();

		public:
			Server(const conf::ServerConfig *config, int client_fd);
			~Server();

			int accepter();
			void handler();
			int responder();

			string						get_headers();
			string						get_content();
			const conf::ServerConfig	*get_config();
			int							get_socket();
			int							get_content_length();
			ServerStatus				get_status();

			void						set_status(ServerStatus status);
	};
}

#endif