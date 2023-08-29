#include "test.hpp"
#include <cstring>

namespace HDE
{
	test::test() : SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
	{
		launch();
	}

	void test::accepter()
	{
		struct sockaddr_in address = get_socket()->get_adddress();
		int addrlen = sizeof(address);
		newsocket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);

		read(this->newsocket, buffer, 30000);
	}

	void test::handler()
	{
		std::cout << buffer << std::endl;
	}

	void test::responder()
	{
		char *hello = "Hello from serverdasdasd";
		write (this->newsocket, hello, strlen(hello));
		close(this->newsocket);
	}

	void test::launch()
	{
		while (true)
		{
			std::cout << "===Waiting===" << std::endl;
			accepter();
			handler();
			responder();
			std::cout << "===Done===" << std::endl;

		}
	}
} 