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
#include <fcntl.h>
// #include <bits/stdc++.h>

const int BUFFER_SIZE = 1024;

namespace HDE
{
	string test::headers;
	string test::content;
	// char test::buffer[30000];

	test::test() : SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
	{
		launch();
	}

	void test::accepter()
	{
		struct sockaddr_in address = get_socket()->get_address();
		int addrlen = sizeof(address);
		this->newsocket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);

		if (this->newsocket < 0)
		{
			std::cerr << "Accept failed" << endl;
			return;
		}

		string request;
		char buffer[BUFFER_SIZE];
		int bytesRead;
		while ((bytesRead = read(this->newsocket, buffer, sizeof(buffer))) > 0)
		{
			request.append(buffer, bytesRead);
			size_t pos = request.find("\r\n\r\n");
			if (pos != string::npos)
			{
				headers = request.substr(0, pos + 4);
				// cout << "Received Headers:\n" << headers << endl;
				content = request.substr(pos + 4);
				// If Content-Length header is present, continue reading until the specified length
				string contentLengthHeader = "Content-Length: ";
				size_t contentLengthPos = headers.find(contentLengthHeader);
				if (contentLengthPos != std::string::npos)
				{
					size_t endOfContent = contentLengthPos + contentLengthHeader.length();
					size_t endOfLine = headers.find("\r\n", endOfContent);
					int contentLength = std::stoi(headers.substr(endOfContent, endOfLine - endOfContent));
					while (content.size() < contentLength)
					{
						bytesRead = read(this->newsocket, buffer, sizeof(buffer));
						if (bytesRead > 0)
							content.append(buffer, bytesRead);
						else
							break;
					}
				}
				// cout << "Received Content:\n" << content << endl;
				break;
			}
		}
	}

	void test::handler()
	{
		// cout << this->headers << endl << endl << endl;;
		// cout << this->content << endl;
	}

	void test::responder()
	{
		// std::string hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nTesting";
		// sendData(this->newsocket, (void *)hello.c_str(), hello.size());
		dataSet(this->newsocket);
		dataGet();
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

	string test::get_headers()
	{
		return headers;
	}

	string test::get_content()
	{
		return content;
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

void dataGet()
{
	string headers = HDE::test::get_headers();
	string content = HDE::test::get_content();
	string boundary, filename;

	if (headers.find("POST") != string::npos)
	{
		size_t boundaryPos = headers.find("boundary=");
		boundary = headers.substr(boundaryPos + 9);
		boundary = boundary.substr(0, boundary.find("\r"));

		size_t filenamePos = content.find("filename=");
		filename = content.substr(filenamePos + 10);
		filename = filename.substr(0, filename.find("\""));

		cout << "Boundary: |" << boundary << "|" << endl;
		cout << "Filename: |" << filename << "|" << endl;

		size_t dataPos = content.find("\r\n\r\n") + 4;
		size_t boundaryPosInData = content.find("--" + boundary, dataPos);
		std::string fileData = content.substr(dataPos, boundaryPosInData - dataPos);
		std::ofstream outFile(filename.c_str(), std::ios::binary);
		outFile.write(fileData.c_str(), fileData.length());
		outFile.close();
	}

}

void dataSet(int socket)
{
	// std::vector<string> buffer = HDE::test::get_bufferVEC();

	string headers = HDE::test::get_headers();
	string content = HDE::test::get_content();
	string file = "../html/200.html", path;

	// for (std::vector<string>::iterator it = buffer.begin(); it != buffer.end(); it++)
	// {
	file = "../html/200.html";
	if (headers.find("GET") != string::npos)
	{
		path = headers.substr(headers.find("GET ") + 4);
		path = path.substr(0, path.find(" "));
		string path2 = ".." + path;
		if (path.find("/component/error.css") != string::npos)
			file = "../html" + path;
		if (access(path2.c_str(), F_OK) == 0 && path2 != "../")
			file = ".." + path;
	}

	cout << "file: " << file << endl;
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
}