#include "BindingSocket.hpp"

namespace HDE
{
	BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) :
		SimpleSocket(domain, service, protocol, port, interface)
	{
		// set_connection(connect_to_network(get_sock(), get_adddress()));
		connect_to_network(get_sock(), get_adddress());
		test_connection(this->binding);
	}

	// BindingSocket::~BindingSocket()
	// {
	// }

	void BindingSocket::connect_to_network(int sock, struct sockaddr_in address)
	{
		// return (bind(sock, (struct sockaddr *)&address, sizeof(åaddress)));
		this->binding = bind(sock, (struct sockaddr *)&address, sizeof(address));
	}

	int BindingSocket::get_binding()
	{
		return(binding);
	}

}