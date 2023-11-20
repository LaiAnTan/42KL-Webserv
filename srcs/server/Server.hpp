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

// const int BUFFER_SIZE = 16000;
const int BUFFER_SIZE = 20;

using std::string;

namespace conf
{
	class ServerConfig;
}

namespace HDE
{
	enum ServerStatus {
		NEW,
		GET_HEADER,
		PROCESS_HEADER,
		SENDING_RESPONSE,
		SEND_AUTO_INDEX,
		SEND_ERROR,
		SEND_CHUNK,
		SAVE_CHUNK,
		CLEARING_SOCKET,
		DONE
	};

	enum AccepterStatus {
		ACP_ERROR = -1,
		ACP_DISCONNECT = 0,
		ACP_SUCCESS = 1,
		ACP_FINISH = 2
	};

	enum ResponderStatus {
		RES_ERROR = -1,
		RES_SUCCESS = 0,
		RES_FINISH = 1
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
			string						location_config_path;

			int		read_header();
			void	parse_header();
			int		check_valid_method();
			void	find_config_location();
			void	decode_path();

			ServerStatus				status;

			// guys get and post and delete should be an abstract class
			// this is so bad :D

			// error
			string						error_code;

			// get 
			string						real_filepath;
			string						redirect_url;
			bool						auto_index;

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

			double	convert_content_length(string suffix);// converts content length to the specified suffix

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
			string	decode_data(const string &source);
			void	determine_send_type();

			// for cgi
			int	py();
			std::vector<std::pair<string, string> >	extract_cgi_variable(const string &source);

			int		send_next_chunk();
			int		import_read_data();
			string	encode_url(const string &value);

			// reset
			void	reset();


			// index generator
			int		generate_index();

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

			// error handling
			void	no_response();
	};
}

#endif