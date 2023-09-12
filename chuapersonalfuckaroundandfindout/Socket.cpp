#include "Socket.hpp"

namespace HDE
{
	// hihi, change this value to idk, lol
	int	Socket::backlog = 5;

	// we pray to god that by the time we submit webserv, we are still using IPv4
	int	Socket::addressfam = AF_INET;
	int	Socket::socket_type = SOCK_STREAM;
	int	Socket::protocol = 0;

	void	Socket::init_addr(int port)
	{
		// https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html

		address.sin_family = Socket::addressfam;

		// fuck me we have to handle different ports REEEEEEE
		address.sin_port = htons(port);

		// struct in_addr has a single variable
		// unsigned long s_addr (in in_addr_t format)

		/*
			sin_addr -> IP address of the socket
		*/

		/*
		If this field is set to the constant INADDR_ANY, 
		the caller is requesting that the socket be bound 
		to all network interfaces on the host. 
		
		UDP packets and TCP connections 
		from all interfaces (which match the bound name) 
		are routed to the application.
		
		This becomes important when a server offers a service 
		to multiple networks. By leaving the address 
		unspecified, the server can accept all UDP packets 
		and TCP connection requests made for its 
		port, regardless of the network interface
		on which the requests arrived.
		*/

		// i dont think there will be a time where we need to spcify ip address?
		// no info on this :P
		address.sin_addr.s_addr = INADDR_ANY;

		// the last one, char sin_zero[8], is just to pad sizes
		// so it matches struct sockaddr
	}

	// for testing purposes
	void	Socket::init_addr(int port, char *ipv4)
	{
		address.sin_family = Socket::addressfam;
		address.sin_port = htons(port);
		inet_aton(ipv4, &(address.sin_addr));
	}

	// empty socket for your socketing needs
	Socket::Socket() {}

	Socket::Socket(Socket &other)
	{
		(*this) = other;
	}

	Socket	&Socket::operator=(Socket &other)
	{
		if (&other != this)
		{
			this->address = other.address;
			this->addressfam = other.addressfam;
		}
		return (*this);
	}

	// opens a socket for a specific port
	Socket::Socket(int port)
	{
		// intialize a brand new socket
		// used on everything that touches socket actually :P

		// when a socket is created, the program should specify the address domain and the socket type
		// initialize that um, struct sockaddr_in

		this->init_addr(port);

		/*
		// address domain

		// there are two widely used address domains
		//			- unix domain	
		//				- Two processes which share a common file system communicate

		//			- Internet domain (i think we are using this)
		//				- two processes running on 2 host on the internet can communicate

		// address format of the Internet domain

		*/
		
		/*
		// domain - communication domain in which a socket is created
		// aka ADDRESS FAMILY
				- speicfy the type of addresses the socket can communicate with
			- AF_UNIX
				- Denotes the operating system path names.
			- AF_INET
			- AF_INET6
				- Both are for Internet 
				- We are using AF_INET (safest option)
					- AF_IENT6 is for IPv6
				- Difference
					- AF_INET6 is for IPv6 and IPv4
					- AF_INET is for IPv4
					- Why not AF_INET6?
						- IPv6 socket will accept IPv4 packets even if the node has no IPv4 sockets open
							- may allow a malicious IPv4 peer to access a service eve though 
							there is no open IPv4 Socket
						- IPv6 is also not deisnged to be IPv4 compatible 
			- AF_NS
				- wtf is XEROX Network Systems protocol
			- AF_BYPASS
				- kernel-bypass protocol domain whatthefuck?

			// The AF_INET address family
				- Provides interprocess communication between process that runs on the same system or different
				systems

				- IP addresses and Port numbers 
				- uses the sockaddr_in address sturcture

				WHATS IN THE SOCKADDR_IN sturcture

				sin_family - contains the address family
							- always AF_INET (TCP, UDP)
				
				sin_port - contains the port number
				sin_addr - contains the IP Address
				sin_zero - just used to pad the length of the SOCKADDR_IN
						 - you can pad it with zero, or dont, its not necessary 
						 - you can use memset to set it to 0
		// 
		// type - type of socket
		//		- SOCK_STREAM (TCP)
					- reliable, bidirectional, connectio-mode byte streams
					- data read from and written to those sockets as a byte stream
					- shits are delivered in order
					- WE ARE USING THIS HEHEHEHEHEHEHE

					- EVEN MROE INFO ON SOCK_STREAM
						- designed to prevent loss or duplication of data
						- If a piece of data cannot be transmitted within a period, connection breaks
							- indicates error with -1 and errno global variable is ETIMEDOUT

				- SOCK_DGRAM (UDP)
					- sending datagrams
					- delivery, arrivable and order of arrival is not guaranteed
						- receiver need to check for order

				- SOCK_RAW (ICMP)
					- uh idk
					- we cant use jiushile
					- only superuser processes can use this

				- SOCK_SEQPACKET
				- SOCK_CONN_DGRAM
					- yeah, no one cares about this 2

		// protocol - specify a particular protocol to be used for the socket
					- PF_UNIX - local
					- PF_INET - Internet
					- PF_NDD - operating system NDD
		//			- 0 ==> use a default protocol for the requested socket type
						- i let you guess which one is used instead
		//
		*/

		this->socket_fd = socket(Socket::addressfam, Socket::socket_type, Socket::protocol);

		// allow socket to be reusable

		// SO_REUSEADDR ---> allow reuse of local addresses
		int	on = 1;
		if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
		{
			std::cerr << RED << "setsockopt failed" << std::endl;
			this->socket_fd = this->s_close();
			return;
		}

		// set socket to be non blocking

		// a non blocking fd means that read() will immediately return (EAGAIN) if there is no byte to read
		// 	- usually read() will wait until at least one byte is available (blocks)

		// FIONBIO or O_NONBLOCK
		// FIONBIO and O_NDELAY are used b4 O_NONBLOCK
		// - inconsisten between system
		// 		- FIONBIO work on sockets
		//		- O_NDELAY work on ttys
		//
		// O_NONBLOCK is introduced to have a standardized behavior
		// across different systems and file descriptor type
		// https://stackoverflow.com/questions/1150635/unix-nonblocking-i-o-o-nonblock-vs-fionbio

		// int flags = fcntl(this->socket_fd, F_GETFL, 0);

		// yoink from the pdf
		if (fcntl(this->socket_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0)
		{
			std::cerr << RED << "ioctl() failed" << std::endl;
			this->socket_fd = this->s_close();
			return;
		}

		// if (ioctl(this->socket_fd, FIONBIO, (char *)&on) < 0)
		// {
		// 	std::cerr << RED << "ioctl() failed" << std::endl;
		// 	this->socket_fd = this->s_close();
		// 	return;
		// }
	}

	// with specific ip address
	// mainly to test if i implemented Socket properly
	Socket::Socket(int port, char *ipv4)
	{
		this->init_addr(port, ipv4);
		this->socket_fd = socket(Socket::addressfam, Socket::socket_type, Socket::protocol);
		if (this->socket_fd < 0)
		{
			std::cerr << RED << "Unable to create a Socket :(" << endl;
			return;
		}

		int	on = 1;
		if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
		{
			std::cerr << RED << "setsockopt failed" << std::endl;
			this->socket_fd = this->s_close();
			return;
		}

		if (fcntl(this->socket_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0)
		{
			std::cerr << RED << "ioctl() failed" << std::endl;
			this->socket_fd = this->s_close();
			return;
		}
	}

	int		Socket::s_test()
	{
		cout << YELLOW << "Testing Connection..." << endl;

		if (this->socket_fd != -1)
			cout << GREEN << "Connection Success! :)" << endl;
		else
			cerr << RED << "Connection Failed! :(" << endl;
		return	(this->socket_fd != -1);
	}

	int		Socket::s_get_fd()
	{
		return	(this->socket_fd);
	}

	// binds a unique local name to the socket

	// done to use this address/port as the source of the data when sending data
	
	// so if you bind a socket for receiving data to a specific address you can only 
	// receive data sent to this specific IP address
	// example : binding 127.0.0.1 allow you to receive data from your own system
	// but not other systems on the local network
	// 127.0.0.1 is LOCALHOST btw
	// IP address of the local computer (allow you to connect and communicate with itself)
	int		Socket::s_bind()
	{
		cout << YELLOW << "Attempting to bind name to socket" << endl;

										// i shit you not, this is how you do it 
		int ret = bind(this->socket_fd, (const sockaddr *) &this->address, sizeof(address));
		if (ret == 0)
			cout << GREEN << "Binding sucessfull :)" << endl;
		else
		{
			cerr << RED << "Binding Failed :(" << endl;
			cerr << RED << "Reason: " << strerror(errno) << endl;
		}
		return (ret == 0);
	}

	// set socket to listen to incoming client requests
	int		Socket::s_listen()
	{
		cout << YELLOW << "Setting Socket to listen state" << endl;
		int ret = listen(this->socket_fd, Socket::backlog);
		if (ret == 0)
			cout << GREEN << "Socket is now Listening to connection Request!" << endl;
		else
			cerr << RED << "Unable to set Socket to listening :(" << endl;
		return (ret == 0);
	}

	/*
	// ok awkward
	// so accept actually creates a new socket for the client
	// returns the file_descriptor for the client
	// with the same socket type protocol and shit yeah same everything idk wohoo
	*/

	// accepts a connection on the socket
	// returns a new socket to read data from the process that requested connection

	// update - now returns int, i cannot make Socket work
	int	Socket::s_accept()
	{
		int			ret;
		// dont save the thing
		// its fine, the socket will still inherit the sockaddr_t from the caller
		socklen_t	len = 0;

		ret = accept(this->socket_fd, NULL, &len);
		if (ret != -1)
			cout << GREEN << "Connection succesfully accepted!" << endl;

		// great news! we CANT use this :D

		// else
		// 	// socket receive buffer is empty when trying to receive
		// 	// ie - no more to receive
		// 	if (errno != EWOULDBLOCK)
		// 		cerr << RED << "Connection Failed to be accepted, an error occured! :(" << endl; 
		return ret;
	}

	// client side
	// this will, most likely, will not be used :D

	// when the client connect, client will send and receive data from the same socket
	// ...apparantly, idk :P
	int		Socket::s_connect()
	{
		int	ret;
		cout << YELLOW << "Attempting to connect Socket..." << endl;
		ret = connect(this->socket_fd, (const sockaddr *) &this->address, sizeof(address));
		if (ret == 0)
			cout << GREEN << "Connection Successfull! :)" << endl;
		// sockets are non-blocking, EINPROGRESS is completely FINE
		else if (errno == EINPROGRESS)
			cout << YELLOW << "Connection Probably Succeeded..?" << endl;
		else
		{
			cerr << RED << "Connection Failed :(" << endl;
			cout << RED << "Reason " << strerror(errno) << endl;
		}
		cout << RESET << endl;
		return ((ret == 0));
	}

	// cleanup
	int		Socket::s_close()
	{
		if (this->socket_fd > -1)
		{
			cout << BLUE << "Closing Socket..." << endl;
			close(this->socket_fd);
			this->socket_fd = -1;
		}
		return (-1);
	}

	Socket::~Socket()
	{
		this->s_close();
	}
} // namespace HDE
