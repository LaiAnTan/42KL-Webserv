#include "test.hpp"
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <ctime>

const int BUFFER_SIZE = 1024;

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
		int bytesrecv = read(this->newsocket, this->buffer, 30000);
		if (bytesrecv < 0)
			cout << "no bytes received" << endl;
		// std::ostringstream ss;
		// ss << "------ Received Request from client ------\n\n";
		// cout << ss.str() << endl;
	}

	void test::handler()
	{
		// std::cout << buffer << std::endl;
		std::stringstream ss(buffer);
		string key;
		while (std::getline(ss, key, '\n'))
		{
			if (key.empty())
				continue;
			else
				this->bufferVEC.push_back(key);
		}
		// cout << "==========vector start===========" << endl;
		// for (std::vector<string>::iterator it = this->bufferVEC.begin(); it != this->bufferVEC.end(); it++)
		// 	std::cout << *it << std::endl;
		// cout << "=========vector end============" << endl;
	}

	void test::responder()
	{
		// std::string hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTesting";
		// sendData(this->newsocket, (void *)hello.c_str(), hello.size());
		dataSet(this->newsocket);
		// receive(this->newsocket);
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
		char img_buffer[BUFFER_SIZE];
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
		char img_buffer[BUFFER_SIZE];
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

void css(string type, int sock)
{
	string response;
	response.append("HTTP/1.1 200 OK\r\n");
	response.append("Content-Type: text/css\r\n\r\n");
	std::ifstream file;
	file.open(type.c_str());

	if (file.is_open())
	{
		while (!file.eof())
		{
			string css;
			std::getline(file, css);
			response.append(css);
		}
		file.close();
		int res = sendData(sock, (void *)response.c_str(), response.size());
	}
	else
		std::cerr << "Error opening css file." << endl;
}

void html(string type, int sock)
{
	string filename, response, str1, str2;
	std::ifstream file;

	filename = type;
	response.append("HTTP/1.1 200 OK\r\n");
	response.append("Content-Type: text/html\r\n\r\n");
	string code[] = {"400.html", "404.html", "405.html", "413.html", "500.html", "501.html", "505.html"};
	string msg[] = {"Bad Request", "Not Found", "Method Not Allowed", "Payload Too Large", "Internal Server Error", "Not Implemented", "HTTP Version Not Supported"};
	for (int i = 0; i < 7; i++)
	{
		if (type.find(code[i]) != string::npos)
		{
			str1 = code[i].substr(0, code[i].length() - 5);;
			str2 = msg[i];
			filename = "../html/error.html";
		}
	}

	file.open(filename.c_str());
	if (file.is_open())
	{
		while (!file.eof())
		{
			string html;
			std::getline(file, html);
			string find_code = "[CODE]";
			string find_msg = "[MSG]";
			if (html.find(find_code) != string::npos)
				html.replace(html.find(find_code), find_code.length(), str1);
			if (html.find(find_msg) != string::npos)
				html.replace(html.find(find_msg), find_msg.length(), str2);
			response.append(html);
		}
		file.close();
		int res = sendData(sock, (void *)response.c_str(), response.size());
	}
	else
		std::cerr << "Error opening html file." << endl;
}

void login(string type, int sock)
{
    string response;
    // response.append("HTTP/1.1 200 OK\r\n");
    // response.append("Content-Type: text/html\r\n\r\n");

    // Check if the provided username and password are correct (hard-coded for demonstration)
    std::string username = "demo";
    std::string password = "password";

    std::vector<string> buffer = HDE::test::get_bufferVEC();

	// cout << "==========vector start===========" << endl;
	// for (std::vector<string>::iterator it = buffer.begin(); it != buffer.end(); it++)
	// 	std::cout << *it << std::endl;
	// cout << "=========vector end============" << endl;

    // Check if the POST data contains the correct username and password
    if (buffer.size() >= 2 && buffer[buffer.size() - 1].find("username=" + username) != std::string::npos &&
        buffer[buffer.size() - 1].find("password=" + password) != std::string::npos)
    {
        response.append("<h1>Login Successful</h1>");
    }
    else
    {
		response.append("<h1>Login Failed</h1>");
    }

    int res = sendData(sock, (void *)response.c_str(), response.size());
}

void dataSet(int socket)
{
	std::vector<string> buffer = HDE::test::get_bufferVEC();

	string file = "../html/200.html", path;

	for (std::vector<string>::iterator it = buffer.begin(); it != buffer.end(); it++)
	{
		cout << *it << endl;
		if ((*it).find("GET") != string::npos)
		{
			size_t firstSpacePos = (*it).find(' ');
			size_t secondSpacePos = (*it).find(' ', firstSpacePos + 1);
    		path = (*it).substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
			string path2 = ".." + path;
			cout << "path2: " << path2 << endl;
			if (path.find("error.css") != string::npos)
				file = "../html" + path;
			else if (access(path2.c_str(), F_OK) == 0 && path2 != "../")
				file = ".." + path;
			break;
		}
	}

	cout << "file: " << file << endl;
	if (file == "../html/test.html")
	{
		login(file, socket);
	}
	void (*funct[])(string type, int sock) = {&icon, &png, &html, &css};
	string arr[] = {".ico", ".png", ".html", ".css"};

	for (int i = 0; i < 4; i++)
	{
		if (file.find(arr[i]) != string::npos)
		{
			funct[i](file, socket);
			break;
		}
	}

	cout << "==========vector start===========" << endl;
	for (std::vector<string>::iterator it = buffer.begin(); it != buffer.end(); it++)
		std::cout << *it << std::endl;

	cout << "=========vector end============" << endl;
}