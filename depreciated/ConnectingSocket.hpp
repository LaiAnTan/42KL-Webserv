#ifndef ConnectingSocket_HPP
#define ConnectingSocket_HPP

#include "BindingSocket.hpp"

namespace HDE
{
	class ConnectingSocket : public SimpleSocket
	{
	private:
		int connecting;
		void connect_to_network(int sock, struct sockaddr_in address);
	public:
		ConnectingSocket(int domain, int service, int protocol, int port, u_long interface);
		// ~ConnectingSocket();

		int	get_connecting();
	};
}

#endif