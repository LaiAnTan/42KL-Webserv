#include "ConnectingSocket.hpp"

namespace HDE
{
	ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) :
		SimpleSocket(domain, service, protocol, port, interface)
	{
		connect_to_network(get_sock(), get_address());
		test_connection(this->connecting);

	}
	
	// ConnectingSocket::~ConnectingSocket()
	// {	
	// }

	void ConnectingSocket::connect_to_network(int sock, struct sockaddr_in address)
	{
		// return (connect(sock, (struct sockaddr *)&address, sizeof(address)));
		this->connecting = connect(sock, (struct sockaddr *)&address, sizeof(address));
	}

	int ConnectingSocket::get_connecting()
	{
		return (this->connecting);
	}
}