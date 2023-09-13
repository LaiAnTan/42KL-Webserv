#include "Server.hpp"

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
		if (res < 0)
			std::cerr << "Error sending icon file." << endl;
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
		if (res < 0)
			std::cerr << "Error sending png file." << endl;
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
		if (res < 0)
			std::cerr << "Error sending css file." << endl;
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
			filename = "./html/error.html";
		}
	}
	file.open( filename.c_str());
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
		if (res < 0)
			std::cerr << "Error sending html file." << endl;
	}
	else
		std::cerr << "Error opening html file." << endl;
}

void dataSet(int socket)
{
	// std::vector<string> buffer = HDE::Server::get_bufferVEC();

	string headers = HDE::Server::get_headers();
	string content = HDE::Server::get_content();
	string file = "./html/200.html", path;

	// for (std::vector<string>::iterator it = buffer.begin(); it != buffer.end(); it++)
	// {
	file = "./html/404.html";
	if (headers.find("GET") != string::npos)
	{
		path = headers.substr(headers.find("GET ") + 4);
		path = "." + path.substr(0, path.find(" "));
		if (path == "./")
			file = "./html/404.html";
		else if (access(path.c_str(), R_OK) == 0)
			file = path;
	}
	cout << "File: " << file << "	Path: " << path << endl;
	void (*funct[])(string type, int sock) = {&icon, &png, &css, &html};
	string arr[] = {".ico", ".png", ".css", ".html"};

	for (int i = 0; i < 4; i++)
	{
		if (file.find(arr[i]) != string::npos)
		{
			funct[i](file, socket);
			break;
		}
	}
}