#include "test.hpp"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>

namespace HDE
{
	std::vector<string> test::bufferVEC;

	test::test() : SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
	{
		launch();
	}

	void test::accepter()
	{
		struct sockaddr_in address = get_socket()->get_adddress();
		int addrlen = sizeof(address);
		newsocket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
		bufferVEC.clear();
		read(this->newsocket, buffer, 30000);
	}

	void test::handler()
	{
		std::cout << buffer << std::endl;
		std::stringstream ss(buffer);
		string key;
		while (std::getline(ss, key, '\n'))
		{
			if (key.empty())
				continue;
			else
				this->bufferVEC.push_back(key);
		}
		cout << "==========vector start===========" << endl;
		for (std::vector<string>::iterator it = this->bufferVEC.begin(); it != this->bufferVEC.end(); it++)
			std::cout << *it << std::endl;
		cout << "=========vector end============" << endl;
	}

	void test::responder()
	{
		// std::string hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTesting";
		// sendData(this->newsocket, (void *)hello.c_str(), hello.size());
		string	test = "test";
		dataSet(this->newsocket);
		// write (this->newsocket, hello, strlen(hello));
		close(this->newsocket);
	}

	void test::launch()
	{
		while (true)
		{
			std::cout << "===Waiting===" << std::endl;
			accepter();
			std::cout << "===Accepted===" << std::endl;
			handler();
			std::cout << "===Handled===" << std::endl;
			responder();
			std::cout << "===Done===" << std::endl;

		}
	}

	std::vector<string> test::get_bufferVEC()
	{
		return (bufferVEC);
	}
}

int sendData(int sckt, const void *data, int datalen)
{
    const unsigned char *pdata = (const unsigned char *) data;
    int numSent;

	while (datalen > 0)
	{
		numSent = send(sckt, pdata, datalen, 0);
		if (numSent == -1) return -1;
		pdata += numSent;
		datalen -= numSent;
	}
	return 0;
}

void icon(string type, int sock)
{
	string response;
	response.append("HTTP/1.1 200 OK\r\n");
	response.append("Content-Type: image/x-icon\r\n\r\n");
	std::ifstream file;
	file.open(type.c_str());

	if (file.is_open())
	{
		char img_buffer[1024];
		while (file.read(img_buffer, sizeof(img_buffer)))
			response.append(img_buffer, sizeof(img_buffer));
		if (file.eof())
			response.append(img_buffer, file.gcount());
		file.close();
		int res = sendData(sock, (void *)response.c_str(), response.size());
	}
	else
		std::cerr << "Error opening icon file." << endl;
}

void png(string type, int sock)
{
	string response;
	response.append("HTTP/1.1 200 OK\r\n");
	response.append("Content-Type: image/png\r\n\r\n");
	std::ifstream file;
	file.open(type.c_str());

	if (file.is_open())
	{
		char img_buffer[1024];
		while (file.read(img_buffer, sizeof(img_buffer)))
			response.append(img_buffer, sizeof(img_buffer));
		if (file.eof())
			response.append(img_buffer, file.gcount());
		file.close();
		int res = sendData(sock, (void *)response.c_str(), response.size());
	}
	else
		std::cerr << "Error opening png file." << endl;
}

void html(string type, int sock)
{
	string response;
	response.append("HTTP/1.1 200 OK\r\n");
	response.append("Content-Type: text/html\r\n\r\n");
	std::ifstream file;
	file.open(type.c_str());

	if (file.is_open())
	{
		char html[1024];
		while (file.read(html, sizeof(html)))
			response.append(html, sizeof(html));
		if (file.eof())
			response.append(html, file.gcount());
		file.close();
		int res = sendData(sock, (void *)response.c_str(), response.size());
	}
	else
		std::cerr << "Error opening html file." << endl;
}

void dataSet(int socket)
{
	std::vector<string> buffer = HDE::test::get_bufferVEC();

	cout << "input buffer:";
	cout << buffer.front() << endl;

	string file = "../html/404.html";


	void (*funct[])(string type, int sock) = {&icon, &png, &html};
	string arr[] = {".ico", ".png", ".html"};

	for (int i = 0; i < 3; i++)
	{
		if (file.find(arr[i]) != string::npos)
		{
			funct[i](file, socket);
			break;
		}
	}

}