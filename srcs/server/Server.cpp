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
		this->status = NEW;
		this->content_length = -1;
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
		double				limit;
		double				converted;
		int					bytesRead;
		char				buffer[BUFFER_SIZE];
		string				client_max_body_size;
		string				suffix;
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

		// why would you check if content_length == -1 peko
		// get content length
		this->content_length = extract_content_length(headers);
		this->content_length -= this->content.length();
		
		// handle client_max_body_size
		client_max_body_size = config->get_client_max();
		suffix = client_max_body_size.substr(client_max_body_size.size() - 2);

		if (not (suffix == "KB" || suffix == "MB" || suffix == "GB"))
		{
			if (client_max_body_size.substr(client_max_body_size.size() - 1) == "B")
				suffix = client_max_body_size.substr(client_max_body_size.size() - 1);
			else
				throw (conf::InvalidSuffixException());
		}
		
		limit = std::strtof(client_max_body_size.substr(0, client_max_body_size.find(suffix)).c_str(), NULL);

		converted = convert_content_length(suffix);

		if (converted > limit)
		{
			// do something here
			cout << "Over the limit, "  << converted << " > " << limit << endl;
			sendError("413");
		}

		return headers.length() + content.length();
	}

	void	Server::parse_header()
	{
		string						header = get_headers();
		string						first_row = util::split(header, "\r\n")[0];
		std::vector<std::string>	first_row_info = util::split_many_delims(first_row, " ");

		// cout << first_row << endl;
		// cout << first_row_info[0] << endl;

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

		if (location.find(this->path) != location.end())
		{
			location_rules = location[this->path].get_rules();

			if (location_rules.find("allowed_methods") != location_rules.end())
				location_methods = location_rules["allowed_methods"];
			else
				// since dont have specific, use the server wide one
				// technically, this can go below the else statement
				// but the code would look absolutely horrendous
				location_methods = server_methods;
		}
		else
		{
			// i honestly do not know what to do with this
			// probably use the default one ig
			location_methods = server_methods;
		}

		// check if method is allowed
		// where the fuck is auto when u need it
		std::vector<string>::iterator	method_start = location_methods.begin();
		for (; method_start != location_methods.end(); ++method_start)
		{
			// all good!
			// ps - return -25 since if it isnt found, sendError will be sent instead
			// and sendError has a return value of 0 and 1
			// sorry bout that :(
			if (*(method_start) == this->method){
				return -25;
			}
		}
		// nope nope this method is NOT allowed
		// return sendError
		return sendError("405");
	}

	int Server::responder()
	{
		string	header;
		int		ret_value = 1;

		// you know these could have been in a abstract class riiiight?
		function_ptr	function_list[3] = {&Server::handleGetRequest, &Server::handlePostRequest, &Server::handleDeleteRequest};
		string			method_list[3] = {"GET", "POST", "DELETE"};
		// cout << "Server status --- " << this->status << endl;
		switch (this->status)
		{
			case NEW:
				header = get_headers();
				this->parse_header();

				// check if method is valid
				ret_value = this->check_valid_method();
				if (ret_value != -25)
					break;

				// switch does not work on string
				// this is one way of finding out
				for (int x = 0; x < 3; ++x)
				{
					if (this->method == method_list[x]){
						ret_value = (this->*function_list[x])();
					}
				}
				break;
			case SENDING_DATA:
				ret_value = this->send_next_chunk();
				break;
			case RECEIVING_DATA:
				ret_value = this->import_read_data();
				break;
			case DONE:
				// which reminds me, we should empty the socket 
				// after everything is done

				// because acceptor will only read the header, not the
				// content

				// content will be left in the socket
				// at least i think it will, not sure

				// actually ukw, lets see if this is the case anot
				// this->empty_socket();

				this->status = NEW;
				break;
			default:
				// not handled LOL
				break;
		}
		return ret_value;
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