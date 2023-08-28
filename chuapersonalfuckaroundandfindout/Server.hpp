#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Client.hpp"

namespace HDE
{
	class Server
	{
		private:
			Socket	server_socket;

		public:
			Server();
			Server(int port);
			Server(int port, char *ip);
			~Server();
			Server	&operator=(Server &other);

			int		SetUp_Socket();
			int		CreateClient();
			Socket	&get_Socket();
	};	
}

#endif