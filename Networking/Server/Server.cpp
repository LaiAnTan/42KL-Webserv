#include "Server.hpp"

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

namespace HDE
{
	string Server::headers;
	string Server::content;

	Server::Server() : SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
	{ 
		launch();
	}

	void Server::accepter()
	{
		struct sockaddr_in address = get_socket()->get_address();
		int addrlen = sizeof(address);
		this->newsocket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
		if (this->newsocket < 0)
		{
			std::cerr << "Accept failed" << endl;
			return;
		}

		string request;
		char buffer[BUFFER_SIZE];
		int bytesRead;
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
	}

	void Server::handler()
	{
		// cout << this->headers << endl;
		// cout << this->content << endl;
	}

	void Server::responder()
	{
		dataSet(this->newsocket);
		dataGet(this->newsocket);
		close(this->newsocket);
	}

	void Server::launch()
	{
		while (true)
		{
			std::cout << "===Waiting===" << std::endl;
			accepter();
			std::cout << "===Accepted===" << std::endl;
			handler();
			std::cout << "===Handled===" << std::endl;
			responder();
			std::cout << "===Done===" << std::endl;

		}
	}

	string Server::get_headers()
	{
		return (headers);
	}

	string Server::get_content()
	{
		return (content);
	}
}

int sendData(int sckt, const void *data, int datalen)
{
	const unsigned char *pdata = (const unsigned char *) data;
	int numSent;

	while (datalen > 0)
	{
		numSent = send(sckt, pdata, datalen, 0);
		if (numSent == -1)
			return -1;
		pdata += numSent;
		datalen -= numSent;
	}
	return 0;
}