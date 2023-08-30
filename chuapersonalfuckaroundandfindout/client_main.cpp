# include "Socket.hpp"
#include <poll.h>
# include "unistd.h"
# include <string>
# include <iostream>

using std::string;
using std::getline;
using std::cout;
using std::endl;
using std::cin;

int	get_response(HDE::Socket &test)
{
	char	buffer[1000];
	int	ret = recv(test.s_get_fd(), buffer, 1000, 0);
	int	fail = false;
	if (ret == 0)
	{
		cerr << RED << "Server Is Closed" << RESET << endl;
		fail = true;
	}
	else if (ret == -1)
	{
		if (errno == EAGAIN)
		{
			cout << YELLOW << "Server has yet to send anything, try again :P" << endl;
		}
		else
		{
			cerr << RED << "Error Happened" << RESET << endl;
			cerr << RED << "Reason: " << strerror(errno) << RESET << endl;
			fail = true;
		}
	}
	else
	{
		cout << GREEN << "Response Gotten!" << RESET << endl;
		buffer[ret] = '\0';
		cout << GREEN << "Server Response: " << RESET << buffer << endl;
	}
	cout << RESET << endl;
	return (fail);
}

int	send_msg(string &str, HDE::Socket &sender)
{
	const char	*ptr;
	int	i = 0;
	char	buff[1024];

	ptr = str.c_str();

	for (;(*ptr) != 0; ++ptr, ++i)
	{
		buff[i] = (*ptr);
	}
	send(sender.s_get_fd(), buff, i, 0);
	send(sender.s_get_fd(), "\0", 1, 0);
	return (1);
}

int main()
{
	HDE::Socket	sender(6969, "127.0.0.1");
	struct pollfd fd;

	fd.fd = sender.s_get_fd();
	fd.events = POLLIN;

	sender.s_connect();
	string	buff;
	while (poll(&fd, 1, 10000000))
	{
		if (fd.revents & POLLIN)
		{
			if (get_response(sender))
				break;
			fd.events = POLLOUT;
		}
		else if (fd.revents & POLLOUT)
		{
			cout << RESET << "Input String: ";
			getline(cin, buff);
			send_msg(buff, sender);
			fd.events = POLLIN;
		}
		else
		{
			cerr << RED << "Welll something went wrong" << endl;
			break;
		}
	}

	// end of connection
	sender.s_close();
}