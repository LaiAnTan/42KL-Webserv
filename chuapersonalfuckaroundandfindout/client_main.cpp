# include "Socket.hpp"
# include "unistd.h"
# include <string>
# include <iostream>

using std::string;
using std::getline;
using std::cout;
using std::endl;
using std::cin;

void	wait_response(HDE::Socket &test)
{
	char	buffer[1000];
	if (recv(test.s_get_fd(), NULL, 1000, 0) == 0)
	{
		cerr << RED << "Server Is Closed" << RESET << endl;
	}
	else
		cout << GREEN << "Response Gotten!" << RESET << endl;
	return ;
}

int main()
{
	HDE::Socket	sender(69, "127.0.0.69");

	sender.s_connect();

	cout << "Input: ";
	for (string store; getline(cin, store);)
	{
		// send the '\0' also
		send(sender.s_get_fd(), store.c_str(), (store.length() + 1), 0);
		wait_response(sender);
		cout << "Input: ";
	}

	// end of connection
	sender.s_close();
}