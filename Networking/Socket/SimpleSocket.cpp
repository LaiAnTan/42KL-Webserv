#include "SimpleSocket.hpp"

namespace HDE
{

	SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long interface)
	{
		//Define socket address struct
		address.sin_family = domain;
		address.sin_port = htons(port);
		address.sin_addr.s_addr = htonl(interface);
		//Establish Socket 
		this->sock = socket(domain, service, protocol);
		const int enable = 1;
		if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    		std::cerr << "setsockopt(SO_REUSEADDR) failed" << endl;
		test_connection(this->sock);
		//Establish Connection
		// std::cout << "run1" << std::endl;
		// this->connection = connect_to_network(this->sock, this->address);
		// std::cout << "run2" << std::endl;
		// test_connection(connection);
	}

	// SimpleSocket::~SimpleSocket()
	// {
	// }

	void SimpleSocket::test_connection(int test)
	{
		if (test < 0)
		{
			perror("Connection test failed! ......");
			exit(EXIT_FAILURE);
		}
	}

	struct sockaddr_in SimpleSocket::get_address()
	{
		return (this->address);
	}
	int  SimpleSocket::get_sock()
	{
		return (this->sock);
	}
	// int  SimpleSocket::get_connection()
	// {
	// 	return (this->connection);
	// }

	// void SimpleSocket::set_connection(int c)
	// {
	// 	this->connection = c;
	// }

}