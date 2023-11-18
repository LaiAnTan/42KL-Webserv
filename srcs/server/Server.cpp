#include "Server.hpp"
#include <sys/socket.h>
#include "../config/Config.hpp"
#include "../util/Utils.hpp"

/*
AF_INET: IPv4
SOCK_STREAM: TCP. Is a connection-based protocol. The connection is established 
	and the two parties have a conversation until the connection is terminated by
	one of the parties or by a network error.
0: Protocol value for Internet Protocol (IP)
80: Port number
INADDR_ANY: IP address of the host. INADDR_ANY means that the server will bind
	to all network interfaces on the host, and receive packets destined to one of
	them. If the server has multiple network interfaces (e.g. an Ethernet and a
	WiFi connection), it will accept packets destined to either one of them.
10: Maximum number of pending connections in the socket's listen queue
*/

using std::string;
using std::endl;
using std::cout;

namespace HDE
{
	double	Server::convert_content_length(string suffix)
	{
		double								converted;
		std::vector<string>					suffixes;
		std::vector<string>::const_iterator	it;

		suffixes.push_back("B");
		suffixes.push_back("KB");
		suffixes.push_back("MB");
		suffixes.push_back("GB");

		it = std::find(suffixes.begin(), suffixes.end(), suffix);
		if (it == suffixes.end())
			return (-1);
		
		converted = this->content_length;

		for (std::ptrdiff_t i = 0; i < it - suffixes.begin(); ++i)
			converted /= 1000;

		return (converted);
	}

	int	extract_content_length(string header)
	{
		std::stringstream	ss;
		int					content_length = 0;
		string				content_length_header_identifier = "Content-Length: ";
		size_t				start_index = header.find(content_length_header_identifier);
		size_t				end_index;
		size_t				end_of_line_index;

		if (start_index != std::string::npos)
		{
			end_index = start_index + content_length_header_identifier.length();
			end_of_line_index= header.find("\r\n", end_index);

			ss << header.substr(end_index, end_of_line_index - end_index);
			ss >> content_length;
		}
		return (content_length);
	}

	Server::Server(const conf::ServerConfig *config, int client_fd)
	{
		this->newsocket = client_fd;
		this->config = config;

		this->reset();
	}

	// curse ass shit
	void	Server::reset()
	{
		this->headers = "";
		this->content = "";

		this->auto_index = false;

		this->status = NEW;
		this->content_length = -1;

		this->method = "";
		this->path = "";
		this->location_config_path = "";

		this->error_code = "";
		this->real_filepath = "";
		this->redirect_url = "";

		chunk_to_send.clear();
		file.close();
		this->no_clear_socket = false;
		save_to.close();
		this->boundary_string = "";

		this->is_deleted = false;
	}

	Server::~Server()
	{
	}

	int	Server::get_socket()
	{
		return this->newsocket;
	}

	int Server::accepter()
	{
		int					bytesRead;
		char				buffer[BUFFER_SIZE];
		string				request;

		// clear previous contents (what the fuck guys why wasnt this cleared?)
		headers.clear();
		content.clear();
		
		// read header
		while (headers.empty() == true)
		{
			bytesRead = read(this->newsocket, buffer, sizeof(buffer));
			if (bytesRead == -1 || bytesRead == 0)
				break;
			request.append(buffer, bytesRead);
			size_t pos = request.find("\r\n\r\n");
			if (pos != string::npos)
			{
				headers = request.substr(0, pos + 4);
				if (pos + 4 < request.length())
					content = request.substr(pos + 4); // retrieve any content that was accidentally extracted as well yes
			}
		}

		// get content length
		this->content_length = extract_content_length(headers);
		this->content_length -= this->content.length();

		return headers.length() + content.length();
	}

	void	Server::parse_header()
	{
		string						header = get_headers();
		string						first_row = util::split(header, "\r\n")[0];
		std::vector<std::string>	first_row_info = util::split_many_delims(first_row, " ");

		this->method = first_row_info[0];
		this->path = first_row_info[1];
	}

	typedef	int (Server::*function_ptr) ();

	int	Server::check_valid_method()
	{
		// general server allowed methods
		std::vector<string>						server_methods = config->get_methods();

		// specific location allowed methods
		std::map<string, conf::ServerLocation>	location = config->get_locations();
		conf::ServerLocation::rules_map			location_rules;
		std::vector<string>						location_methods;

		if (location.find(this->location_config_path) != location.end())
			location_methods = location[this->location_config_path].get_allowed_method();

		if (location_methods.empty() == true)
			location_methods = server_methods; // use default if not found

		// check if method is allowed
		// where the fuck is auto when u need it
		std::vector<string>::iterator	method_start = location_methods.begin();
		for (; method_start != location_methods.end(); ++method_start)
		{
			// all good!
			if (*(method_start) == this->method){
				return 1;
			}
		}
		// nope nope this method is NOT allowed
		// return sendError
		cout << YELLOW << "[INFO] Can't do that [" << this->method << "]" << endl;
		this->error_code = "405";
		return 0;
	}

	void Server::find_config_location()
	{
		std::map<string, conf::ServerLocation>				location = config->get_locations();
		std::string											check(this->path), location_path, location_start;

		for (std::size_t next_slash = check.rfind('/'); 
			(not check.empty()) and (next_slash != string::npos); 
			check = check.substr(0, next_slash), next_slash = check.rfind('/'))
		{
			location_path = check;
			cout << "Checking Path: " << check << endl;
			if (location.find(check) != location.end())
			{
				cout << "Found it" << endl;
				break;
			}
		}
		if (check.empty())
			location_path = "/";
		this->location_config_path = location_path;
		cout << "Location Configuration Used: " << this->location_config_path << endl;
	}

	int Server::responder()
	{
		int		ret_value = 0;

		// you know these could have been in a abstract class riiiight?
		function_ptr	request_list[3] = {&Server::handleGetRequest, &Server::handlePostRequest, &Server::handleDeleteRequest};
		// HAH I KNEW THIS WILL COME BACK AND SHOOT ME IN THE FOOT
		function_ptr	response_list[3] = {&Server::handleGetResponse, &Server::handlePostResponse, &Server::handleDeleteResponse};

		string			method_list[3] = {"GET", "POST", "DELETE"};
		// cout << "Server status --- " << this->status << endl;
		switch (this->status)
		{
			case NEW:
				this->parse_header();
				this->find_config_location();
				this->status = HANDLING_DATA;
				if (!this->check_valid_method())
					this->status = CLEARING_SOCKET;
				break;
			case HANDLING_DATA:
				// switch does not work on string
				// this is one way of finding out
				cout << "Handling Header" << endl;
				this->status = CLEARING_SOCKET;
				for (int x = 0; x < 3; ++x)
				{
					if (this->method == method_list[x]){
						ret_value = (this->*request_list[x])();
					}
				}
				break;
			case SENDING_RESPONSE: 
				// MAKE SURE SOCKET IS CLEARED BEFORE SENDING OVER STUFF
				cout << "Sending HTTP response" << endl;
				this->status = DONE;
				for (int x = 0; x < 3; ++x)
				{
					if (this->method == method_list[x]){
						ret_value = (this->*response_list[x])();
					}
				}
				break;
			case SEND_AUTO_INDEX:
				this->status = DONE;
				this->generate_index();
				break;
			case SEND_ERROR:
				cout << "Sending Error Code" << endl;
				this->status = DONE;
				ret_value = this->sendError(this->error_code);
				break;
			case SEND_CHUNK:
				cout << "Sending Next Chunk" << endl;
				ret_value = this->send_next_chunk();
				break;
			case SAVE_CHUNK:
				cout << "Saving Incoming Chunk" << endl;
				ret_value = this->import_read_data();
				break;
			case DONE:
				this->reset();
				this->status = NEW;
				break;
			case CLEARING_SOCKET:
				cout << "Flushing Socket" << endl;
				if (this->clear_read_end())
				{
					// socket is cleared, clear to send data
					this->status = SENDING_RESPONSE;
					if (not error_code.empty())
						this->status = SEND_ERROR;
					if (this->auto_index)
						this->status = SEND_AUTO_INDEX;
				}
				break;
			default:
				// not handled LOL
				break;
		}
		return ret_value;
	}

	// flush socket for next input
	int Server::clear_read_end()
	{
		// not sure if this will slow down anot
		// should be fine

		char	discard[BUFFER_SIZE];
		int		actual_read;

		if (this->content_length <= 0)
			return 1;

		actual_read = read(this->newsocket, discard, BUFFER_SIZE);
		this->content_length -= actual_read;
		return 0;
	}

	string Server::get_headers()
	{
		return (headers);
	}

	string Server::get_content()
	{
		return (content);
	}

	ServerStatus Server::get_status()
	{
		return status;
	}

	void	Server::set_status(ServerStatus new_status)
	{
		this->status = new_status;
	}

	const conf::ServerConfig *Server::get_config()
	{
		return (config);
	}

	int	Server::get_content_length()
	{
		return (content_length);
	}

	int Server::sendData(int sckt, const void *data, int datalen)
	{
		const unsigned char *pdata = (const unsigned char *) data;
		int numSent;

		while (datalen > 0)
		{
			numSent = send(sckt, pdata, datalen, 0); // can do MSG_NOSIGNAL
			if (numSent == -1)
			{
				std::cerr << RED << "Error Sending Data" << endl;
				return -1;
			}
			pdata += numSent;
			datalen -= numSent;
		}
		return 0;
	}
}