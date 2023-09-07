#include "test.hpp"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>

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
		// std::string hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTesting";
		// sendData(this->newsocket, (void *)hello.c_str(), hello.size());
		string	test = "test";
		dataSet(this->newsocket, (void *)test.c_str());
		// write (this->newsocket, hello, strlen(hello));
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

int sendData(int sckt, void *data, int datalen)
{
    unsigned char *pdata = (unsigned char *) data;
    int numSent;

	while (datalen > 0)
	{
		cout << pdata << endl;
		numSent = send(sckt, pdata, datalen, 0);
		if (numSent == -1) return -1;
		pdata += numSent;
		datalen -= numSent;
	}
	return 0;
}

void dataSet(int socket, void *data)
{
	string response;
	response.append("HTTP/1.1 200 OK\r\n");
	response.append("Content-Type: image/png\r\n\r\n");

	std::ifstream file;
	file.open("../google.png");

	if (file.is_open())
	{
		char img_buffer[1024];
		while (file.read(img_buffer, sizeof(img_buffer)))
			response.append(img_buffer, sizeof(img_buffer));
		if (file.eof())
			response.append(img_buffer, file.gcount());
		file.close();
		int res = sendData(socket, (void *)response.c_str(), response.size());
	}
}