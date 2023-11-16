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
#include <algorithm>
#include <sys/wait.h>

const int BUFFER_SIZE = 1024;
// const int BUFFER_SIZE = 5024;

using std::string;

namespace conf
{
	class ServerConfig;
}

namespace HDE
{
	enum ServerStatus {
		NEW,
		HANDLING_DATA,
		SENDING_RESPONSE,
		SEND_ERROR,
		SEND_CHUNK,
		SAVE_CHUNK,
		CLEARING_SOCKET,
		DONE
	};

	class Server
	{
		private:

			int							newsocket;
			const conf::ServerConfig	*config;

			string						headers;
			string						content;

			// header details
			// we would NOT have this problem IF SOMEONE COMPILED THE HEADER BEFOREHAND
			string						method;
			string						path;

			void	parse_header();
			int		check_valid_method();
	
			ServerStatus				status;

			// guys get and post and delete should be an abstract class
			// this is so bad :D

			// error
			string						error_code;

			// get 
			string						filename;
			string						redirect_url;

			// chunking
			std::ifstream				file;
			std::stringstream			chunk_to_send;

			// post
			// chunking
			bool						no_clear_socket;
			int							content_length;
			std::fstream				save_to;
			string						boundary_string;

			// delete
			bool						is_deleted;

			// function to send data (response) to client
			int		sendData(int sckt, const void *data, int datalen);

			// post
			int	handlePostRequest();
			int	handlePostResponse();

			int			redirectClient();
			string		config_path();

			// get
			int	handleGetRequest();
			int	handleGetResponse();

			// delete
			int	handleDeleteRequest();
			int	handleDeleteResponse();

			// helper
			int		sendError(string type);
			string	get_type(string extension);
			bool	file_exists(std::string path);
			string	get_file_data(std::string filename);
			int		clear_read_end();

			// for cgi
			int	py();

			int	send_next_chunk();
			int import_read_data();

			// helper 2
			double	convert_content_length(string suffix);// converts content length to the specified suffix

			// reset
			void	reset();

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