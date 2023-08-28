// attempt on socket
#ifndef	SOCKET_HPP
#define SOCKET_HPP

#include <unistd.h>
#include <sys/fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/ioctl.h>
#include "Colors.hpp"

#include "string.h"
#include "errno.h"

using std::cout; 
using std::cerr; 
using std::endl;

/*
SOCKETS 

KILL ME

Referenced by fds and can do read, write, select operation
	- exist as long as fd is not closed
Can be created in pairs, given names and used to connect to other sockets in a communication domain
	- accepting connections, exchanging messages

	- names
		- socket names can be given at the sockaddr structure

- Provide 2-way communication path
- Exist within the communication domains

Transfering data
- just use read and write calls
*/

namespace HDE
{
	class Socket
	{
		private:
			int							socket_fd;

			// the socket address structure used literally EVERYWHERE
			struct sockaddr_in			address;

			static int					backlog;
			static int					addressfam;
			static int					socket_type;
			static int					protocol;

			// initialize the sockaddr_in
			void	init_addr(int port, char *ipv4);
			void	init_addr(int port);
		public:
			Socket();
			Socket(Socket &other);
			Socket(int port, char *ipv4);
			Socket(int port);
			~Socket();
			Socket	&operator=(Socket &other);

			int		s_test();

			// getter
			int		s_get_fd();
			
			// server side
			int		s_bind();
			int		s_listen();
			int		s_accept();

			// client side
			int		s_connect();

			// cleanup
			int 	s_close();

	};
} // namespace HDE

#endif