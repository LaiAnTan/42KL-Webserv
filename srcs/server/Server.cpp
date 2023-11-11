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
	Server::Server(const conf::ServerConfig *config, int client_fd)
	{
		this->newsocket = client_fd;
		this->config = config;
		this->status = NEW;
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

		// clear previous contents (what the fuck guys why wasnt this cleared?)
		headers.clear();
		content.clear();

		while ((bytesRead = read(this->newsocket, buffer, sizeof(buffer))) > 0)
		{
			request.append(buffer, bytesRead);
			size_t pos = request.find("\r\n\r\n");
			if (pos != string::npos)
			{
				headers = request.substr(0, pos + 4);
				// cout << "Received Headers:\n" << headers << endl;
				content = request.substr(pos + 4);
				// If Content-Length header is present, continue reading until the specified length
				string contentLengthHeader = "Content-Length: ";
				size_t contentLengthPos = headers.find(contentLengthHeader);
				if (contentLengthPos != std::string::npos)
				{
					size_t endOfContent = contentLengthPos + contentLengthHeader.length();
					size_t endOfLine = headers.find("\r\n", endOfContent);
					std::stringstream ss;
					ss << headers.substr(endOfContent, endOfLine - endOfContent);
					size_t contentLength;
					ss >> contentLength;
					while (content.size() < contentLength)
					{
						bytesRead = read(this->newsocket, buffer, sizeof(buffer));
						if (bytesRead > 0)
							content.append(buffer, bytesRead);
						else
							break;
					}
				}
				// cout << "Received Content:\n" << content << endl;
				break;
			}
		}
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
			case NEW:
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

	const conf::ServerConfig *Server::get_config()
	{
		return (config);
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