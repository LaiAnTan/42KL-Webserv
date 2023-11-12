#include "Server.hpp"
#include <sys/socket.h>
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
		this->curr_post_file_path = "";
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
		string request;
		char buffer[BUFFER_SIZE];
		int bytesRead;

		// clear previous contents and read full header
		if (this->status == NEW)
		{
			headers.clear();
			content.clear();

			// read the whole header
			while (headers.empty() == true)
			{
				bytesRead = read(this->newsocket, buffer, sizeof(buffer));
				if (bytesRead == -1 || bytesRead == 0)
					break;
				request.append(buffer, bytesRead);
				size_t pos = request.find("\r\n\r\n");
				if (pos != string::npos)
					headers = request.substr(0, pos + 4);
			}
			cout << "Done reading header" << endl;
			this->status = READING_DATA;

			// get content length
			if (this->content_length == -1)
				this->content_length = extract_content_length(headers);

			// read once
			if (static_cast<long unsigned int>(this->content_length) > content.length())
			{
				bytesRead = read(this->newsocket, buffer, sizeof(buffer));
				content.append(buffer);
			}
		}
		cout << "read length = " << headers.length() + content.length() << endl;

		return headers.length() + content.length();
	}

	void Server::handler()
	{
		cout << this->headers << endl;
	}

	int Server::responder()
	{
		string	header;
		int ret_value = 0;

		switch (this->status)
		{
			case READING_DATA:
				header = get_headers();

				if (header.find("GET") != string::npos)
					ret_value = handleGetRequest();
				else if (header.find("POST") != string::npos)
					ret_value = handlePostRequest();
				else if (header.find("DELETE") != string::npos)
					ret_value = handleDeleteRequest();
				break;
			case SENDING_DATA:
				ret_value = this->send_next_chunk();
				break;
			case DONE:
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
			numSent = send(sckt, pdata, datalen, MSG_NOSIGNAL); // can do MSG_NOSIGNAL
			if (numSent == -1)
				return -1;
			pdata += numSent;
			datalen -= numSent;
		}
		return 0;
	}
}