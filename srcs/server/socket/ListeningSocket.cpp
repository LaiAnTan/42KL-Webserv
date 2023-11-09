#include "ListeningSocket.hpp"

namespace HDE
{
	ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog) : 
		BindingSocket(domain, service, protocol, port, interface)
	{
		this->backlog = backlog;
		startlisten();
		test_connection(this->listening);
	}
	
	// ListeningSocket::~ListeningSocket()
	// {
	// }

	void ListeningSocket::startlisten()
	{
		this->listening = listen(get_sock(), this->backlog);
	}

	int ListeningSocket::get_backlog()
	{
		return (this->backlog);
	}
	int ListeningSocket::get_listening()
	{
		return (this->listening);
	}
}