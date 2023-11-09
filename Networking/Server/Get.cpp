#include "Server.hpp"

#include <string>

// WHERE ARE THE 'Connection: Keep-Alive'
// we will talk about the above when I go to 42 the next time

namespace HDE
{
	void Server::icon(string type, int sock)
	{
		string response;
		response.append("HTTP/1.1 200 OK\r\n");
		response.append("Connection: close\r\n");
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

	void Server::png(string type, int sock)
	{
		string response;
		response.append("HTTP/1.1 200 OK\r\n");
		response.append("Connection: close\r\n");
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

	void Server::css(string type, int sock)
	{
		string response;
		response.append("HTTP/1.1 200 OK\r\n");
		response.append("Connection: close\r\n");
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

	void Server::html(string type, int sock)
	{
		string filename, response, str1, str2;
		std::ifstream file;

		filename = type;
		response.append("HTTP/1.1 200 OK\r\n");
		response.append("Connection: close\r\n");
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

	string find_bin()
	{
		char *value = getenv("PATH");
		std::stringstream ss(value);
		string var1, path;
		while (std::getline(ss, var1, ':'))
		{
			if (var1.empty())
				continue;
			path = var1 + "/python3";
			if(access(path.c_str(), R_OK) == 0)
				break;
		}
		return (path);
	}

	string data(string filename)
	{
		string response;
		std::ifstream file;

		file.open(filename.c_str());
		if (file.is_open())
		{
			char img_buffer[BUFFER_SIZE];
			while (file.read(img_buffer, sizeof(img_buffer)))
				response.append(img_buffer, sizeof(img_buffer));
			if (file.eof())
				response.append(img_buffer, file.gcount());
			file.close();
		}
		else
			std::cerr << "Error opening png file." << endl;
		return (response);
	}

	void Server::py(string type, int socket)
	{
		(void)type;
		string exe_path = find_bin();	
		std::map<string, string> cgi_vec = config->get_cgi();

		int stdout_fd = dup(1), stdin_fd = dup(0);
		string content;

		int pipe_fd[2];
		if (pipe(pipe_fd) == -1)
		{
			std::cerr << "pipe failed" << std::endl;
			return;
		}

		int pid = fork();
		if (pid == 0)
		{
			string cgi_path = cgi_vec[".py"];

			dup2(pipe_fd[1], 1);
			close(pipe_fd[0]);

			string get = data("./html/file.html");
			std::vector<char *> env_vec;
			env_vec.push_back(strdup(string("CONTENT_TYPE=text/html").c_str()));
			env_vec.push_back(strdup(string("CONTENT_LENGTH=" + std::to_string(get.length())).c_str()));
			env_vec.push_back(strdup(string("first_name=First").c_str()));
			env_vec.push_back(strdup(string("last_name=Last").c_str()));
			// env_vec.push_back(strdup(string("DATA=" + get).c_str()));
			env_vec.push_back(NULL);

			char *args[] = {const_cast<char *>(exe_path.c_str()), const_cast<char *>(cgi_path.c_str()), NULL};
			execve(exe_path.c_str(), args, env_vec.data());
			std::cerr << "execve failed" << std::endl;
			exit(1);
		}
		else if (pid < 0)
			std::cerr << "fork failed" << std::endl;
		else
		{
			close(pipe_fd[1]);

			char buffer[BUFFER_SIZE];
			ssize_t bytes_read;
			while ((bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE)) > 0)
				content.append(buffer, bytes_read);
			close(pipe_fd[0]);
			waitpid(pid, NULL, 0);
		}
		dup2(stdout_fd, 1);
		dup2(stdin_fd, 0);

		string header;

		header.append("HTTP/1.1 200 OK\r\n");
		header.append("Connection: close\r\n");
		// cout << RED << content << RESET << endl;
		sendData(socket, (void *)header.c_str(), header.size());
		sendData(socket, (void *)content.c_str(), content.size());
	}

	void Server::dataSet(int socket)
	{
		string headers = HDE::Server::get_headers();
		string content = HDE::Server::get_content();
		string file = "./html/200.html", path;

		file = "./html/404.html";
		if (headers.find("GET") != string::npos)
		{
			path = headers.substr(headers.find("GET ") + 4);
			path = path.substr(0, path.find(" "));
			cout << YELLOW << path << RESET << endl;
			std::map<string, conf::ServerLocation>::const_iterator it = config->get_locations().begin();
			std::map<string, conf::ServerLocation>::const_iterator end = config->get_locations().end();

			for (; it != end; it++)
			{	
				if (strcmp(path.c_str(), it->first.c_str()) == 0)
				{
					conf::ServerLocation::rules_map::const_iterator rules_it = it->second.get_rules().begin();
					conf::ServerLocation::rules_map::const_iterator rules_end = it->second.get_rules().end();
					for (; rules_it != rules_end; rules_it++)
					{
						if (strcmp("return", rules_it->first.c_str()) == 0)
						{
							cout << RED << rules_it->first << RESET << " ";
							std::vector<string>::const_iterator return_it = rules_it->second.begin();
							std::vector<string>::const_iterator return_end = rules_it->second.end();
							for (; return_it != return_end; return_it++)
							{
								cout << RED << *return_it << RESET << endl;
							}
						}
					}
					break;
				}
			}
			path = "." + path;
			if (path == "./")
				file = "./html/index.html";
			else if (access(path.c_str(), R_OK) == 0)
				file = path;
			else if (path.find(".html") != string::npos)
				file = path;
			else if (path.find("error.css") != string::npos)
				file = "./html/component/error.css";
			else if (path.find(".py") != string::npos)
				file = path;
		}
		cout << GREEN << "File: " << file << "	Path: " << path << RESET<< endl;
		void (Server::*funct[])(string type, int sock) = {&Server::icon, &Server::png, &Server::css, &Server::html, &Server::py};
		string arr[] = {".ico", ".png", ".css", ".html", ".py"};
		for (int i = 0; i < 5; i++)
		{
			if (file.find(arr[i]) != string::npos)
			{
				(this->*funct[i])(file, socket);
				break;
			}
		}
	}
}