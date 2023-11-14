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

const int BUFFER_SIZE = 5024;

using std::string;

namespace conf
{
	class ServerConfig;
}

namespace HDE
{
	enum ServerStatus {
		NEW,
		SENDING_DATA,
		RECEIVING_DATA,
		DONE
	};

	class Server
	{
		private:

			int							newsocket;
			string						headers;
			string						content;
			const conf::ServerConfig	*config;

			// get chunking
			ServerStatus				status;
			std::ifstream				file;
			std::stringstream			chunk_to_send;

			// post chunking
			int							content_length;
			std::fstream				save_to;
			string						boundary_string;

			// function to send data (response) to client
			int		sendData(int sckt, const void *data, int datalen);

			// post
			int	handlePostRequest();
			int	handlePostResponse();

			int redirectClient(string path);

			// get
			int	handleGetRequest();
			int	handleGetResponse(string filename, string redirect_url);

			// delete
			int	handleDeleteRequest();
			int	handleDeleteResponse(bool is_deleted);

			// helper
			int		sendError(string type);
			string	get_type(string extension);
			bool	file_exists(std::string path);
			string	get_file_data(std::string filename);

			// for cgi
			int	py();

			int	send_next_chunk();
			int import_read_data();

		public:
			Server(const conf::ServerConfig *config, int client_fd);
			~Server();

			int accepter();
			int responder();

			string						get_headers();
			string						get_content();
			const conf::ServerConfig	*get_config();
			int							get_socket();
			int							get_content_length();
			ServerStatus				get_status();
			void						set_status(ServerStatus new_status);
	};
}

#endif