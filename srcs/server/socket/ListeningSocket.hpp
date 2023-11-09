#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "BindingSocket.hpp"

namespace HDE
{
	class ListeningSocket : public BindingSocket
	{
	private:
		int backlog;
		int listening;
	public:
		ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog);
		// ~ListeningSocket();
		
		void startlisten();

		int get_backlog();
		int get_listening();
	};
}

#endif 