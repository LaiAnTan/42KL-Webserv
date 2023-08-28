#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Socket.hpp"
#include <string>
#include <unistd.h>

using std::string;

namespace HDE
{
	class Client
	{
		private:
			// since i cant get accept_connection to return Socket
			// this, unfortunately, needs to be in int
			int			client_socket;
			string		content;
			string		to_send;

			static int	buffer_size;
		public:
			Client();
			Client(Client &other);
			Client(int provided_sock);
			~Client();
			
			Client	&operator=(Client &other);

			int		c_read();
			int		c_process();
			int		c_write();
			int		c_write(string &to_write);
			int		c_write(char *to_write);

			int		get_Socket();
			string	&get_Content();
	};
}

#endif 
