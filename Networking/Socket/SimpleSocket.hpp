#ifndef SIMPLESOCKET_HPP
#define SIMPLESOCKET_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sstream>

using std::string;
using std::cout;
using std::endl;

namespace HDE
{
	class SimpleSocket
	{
	private:
		// int connection;
		int sock;
		struct sockaddr_in address;
		
	public:
		SimpleSocket(int domain, int service, int protocol, int port, u_long interface);
		// ~SimpleSocket();

		virtual void connect_to_network(int sock, struct sockaddr_in address) = 0;

		void test_connection(int test);

		int get_sock();
		struct sockaddr_in get_adddress();
		// int get_connection();

		// void set_connection(int c);
	};

}

#endif