#include "ServerListener.hpp"

namespace HDE
{
	ServerListener::ServerListener(const conf::ServerConfig *config, std::string port_str)
	{
		// why is ports an array?
		// well imma handle that later i guess?
		// oh.
		port = std::atoi(port_str.c_str());

		this->server_config = config;
		this->socket = new ListeningSocket(AF_INET, SOCK_STREAM, 0, port, INADDR_ANY, 10);
	}

	ServerListener::~ServerListener()
	{
	}

	int	ServerListener::get_port()
	{
		return (port);
	}

	Server	*ServerListener::accept_connection()
	{
		int	client_fd;

		struct sockaddr_in address = socket->get_address();
		int addrlen = sizeof(address);
		client_fd = accept(socket->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
		if (client_fd < 0)
		{
			std::cerr << "Accept failed" << endl;
			return NULL;
		}

		// note to self, handle leaky leaky later
		return new Server(this->server_config, client_fd);
	}

	int	ServerListener::get_socket()
	{
		return socket->get_sock();
	}
}