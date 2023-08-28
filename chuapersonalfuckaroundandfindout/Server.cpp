#include "Server.hpp"

namespace HDE
{
	Server::Server() : server_socket()
	{
	}

	Server::Server(int port) : server_socket(port)
	{
	}

	Server::Server(int port, char *ip) : server_socket(port, ip)
	{
	}

	Server	&Server::operator=(Server &other)
	{
		if (this != &other)
		{
			this->server_socket = other.server_socket;
		}
		return (*this);
	}

	Socket	&Server::get_Socket()
	{
		return (this->server_socket);
	}

	int	Server::SetUp_Socket()
	{
		this->server_socket.s_bind();
		this->server_socket.s_listen();

		return (this->server_socket.s_test());
	}

	int	Server::CreateClient()
	{
		cout << BLUE << "Detected Client! Connecting Client..." << endl;
		return this->get_Socket().s_accept();;
	}

	Server::~Server()
	{
		this->server_socket.s_close();
	}
}