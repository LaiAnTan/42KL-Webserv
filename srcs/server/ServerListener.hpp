// idk what to name this
// listen to incoming connections, then create a Server object

#ifndef ServerListen_HPP
#define ServerListen_HPP

#include "Server.hpp"
#include "socket/ListeningSocket.hpp"

namespace HDE
{
	class ServerListener
	{
		private:
			ListeningSocket				*socket;
			const	conf::ServerConfig	*server_config;
			int							port;

		public:
			ServerListener(const conf::ServerConfig *config, std::string port_str);
			~ServerListener();

			Server *accept_connection();
			int		get_socket();
			int		get_port();
	};

}

#endif