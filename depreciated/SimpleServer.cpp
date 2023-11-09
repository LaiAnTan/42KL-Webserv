#include "SimpleServer.hpp"

namespace HDE
{
	SimpleServer::SimpleServer(int domain, int service, int protocol, int port, u_long interface, int backlog)
	{
		this->socket = new ListeningSocket(domain, service, protocol, port, interface, backlog);
	}
	
	SimpleServer::~SimpleServer()
	{

	}

	ListeningSocket *SimpleServer::get_socket()
	{
		return (this->socket);
	}
}