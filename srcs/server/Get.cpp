# include "Server.hpp"
# include "../config/Config.hpp"
# include <string>
# include <sstream>
# include <map>
# include <sys/stat.h>

using std::cout;
using std::endl;

namespace HDE
{
	string Server::get_type(string extension)
	{
		std::map<string, string>	mime_type;

		mime_type.insert(std::pair<string, string>(".jpeg", "image/jpeg"));
		mime_type.insert(std::pair<string, string>(".jpg", "image/jpeg"));
		mime_type.insert(std::pair<string, string>(".png", "image/png"));
		mime_type.insert(std::pair<string, string>(".css", "text/css"));
		mime_type.insert(std::pair<string, string>(".html", "text/html"));
		mime_type.insert(std::pair<string, string>(".mp4", "video/mp4"));
		mime_type.insert(std::pair<string, string>(".ico", "image/vnd.microsoft.icon"));

		if (mime_type.find(extension) != mime_type.end())
			return mime_type[extension];
		return "";
	}

	int Server::send_next_chunk()
	{
		char			buffer[BUFFER_SIZE];
		string				sending;
		std::stringstream	chunk_segment;

		if (this->file.is_open())
		{
			if (this->file.read(buffer, BUFFER_SIZE))
				sending.append(buffer, sizeof(buffer));
			if (this->file.eof())
			{
				sending.append(buffer, this->file.gcount());
				this->file.close();
			}
		}

		chunk_segment << std::hex << sending.length() << std::dec << "\r\n";
		chunk_segment << sending << "\r\n";

		cout << YELLOW << "[INFO] Sending Following Content\n" << chunk_segment.str() << RESET << endl; 

		if (sending.length() == 0)
			this->status = DONE;
		else
			this->status = SENDING_DATA;
		return this->sendData(this->newsocket, chunk_segment.str().c_str(), chunk_segment.str().length());
	}

	int Server::handleGetResponse(string filename, string redirect_url)
	{
		string	header, extension;
		std::stringstream	response;
		int		ret_val;

		// for redirects
		if (not redirect_url.empty())
		{
			cout << "Redirection Url = " << redirect_url << endl;
			string response;
			response.append("HTTP/1.1 302 Found\r\n");
			response.append("Location:" + redirect_url + "\r\n\r\n");
			return sendData(this->newsocket, (void *)response.c_str(), response.size());
		}

		// for send file contents
		extension = filename.substr(filename.find(".", 1));

		// handles header
		header.append("HTTP/1.1 200 OK\r\n");
		header.append("Connection: keep-alive\r\n");
		header.append("Content-Type: ");
		header.append(this->get_type(extension));
		header.append("\r\n");

		// determine if chunkey is needed
		struct stat st;

		if (!stat(filename.c_str(), &st))
		{
			int size;

			size = st.st_size;
			// size o file is less than buffer size, can just straight read
			// everything
			if (size < BUFFER_SIZE)
			{
				cout << YELLOW << "SENDING THE ENTIRE FILE AS ONE" << RESET << endl;

				response << header;
				response << "Content-Length: " << size << "\r\n";
				response << "\r\n";
				response << get_file_data(filename.c_str());

				// cout << YELLOW << "response sended: \n" << response.str() << endl;

				ret_val = sendData(this->newsocket, (void *)response.str().c_str(), response.str().size());
				if (ret_val < 0)
					std::cerr << RED << "Error sending file " << filename << endl;
				return ret_val < 0;
			}
			// did not manage to read everything
			// time to chunky chunky
			else
			{
				cout << YELLOW << "[INFO] Chunking" << RESET << endl;
				// send the header stating its using chunkencoding blabla
				header.append("Transfer-Encoding: chunked\r\n");
				header.append("\r\n");

				cout << "Sending Header First -- \n" << header << endl;
				ret_val = sendData(this->newsocket, header.c_str(), header.length());
				this->file.open(filename.c_str());
				if (ret_val < 0)
				{
					std::cerr << RED << "Error sending header for chunking" << endl;
					return 1;
				}
				return this->send_next_chunk();
			}
		}
		else
		{
			std::cerr << RED << "Error opening file " << filename << endl;
			return sendError("404.html");
		}
	}

	// fuck youuu
	int Server::sendError(string type)
	{
		string code[] = {"400.html", "404.html", "405.html", "413.html", "500.html", "501.html", "505.html"};
		string msg[] = {"Bad Request", "Not Found", "Method Not Allowed", "Payload Too Large", "Internal Server Error", "Not Implemented", "HTTP Version Not Supported"};
		string str1, str2, header, error_content;
		std::stringstream	response;

		std::ifstream file;
		for (int i = 0; i < 7; i++)
		{
			if (type.find(code[i]) != string::npos)
			{
				str1 = code[i].substr(0, code[i].length() - 5);;
				str2 = msg[i];
				type = "./html/error.html";
			}
		}

		string find_code = "[CODE]";
		string find_msg = "[MSG]";

		file.open("./html/error.html");
		if (file.is_open())
		{
			// get content
			// and substitue the CODE and MSG in the error.html (why is this a thing?)
			while (!file.eof())
			{
				string html;
				std::getline(file, html);
				if (html.find(find_code) != string::npos)
					html.replace(html.find(find_code), find_code.length(), str1);
				if (html.find(find_msg) != string::npos)
					html.replace(html.find(find_msg), find_msg.length(), str2);
				error_content.append(html);
			}
			file.close();

			// handles header

			response << "HTTP/1.1 " << str1 << " " << str2 << "\r\n";
			response << "Connection: keep-alive\r\n";
			response << "Content-Type: text/html\r\n";
			response << "Content-Length: " << error_content.length() << "\r\n";
			response << "\r\n";
			response << error_content;

			cout << YELLOW << "[INFO] Sending Following Content\n" << response.str() << endl;

			return sendData(this->newsocket, (void *)response.str().c_str(), response.str().size());
		}
		else
			std::cerr << RED << "Error opening error html file." << RESET << endl;
		return 1;
	}

	int Server::redirectClient(string path)
	{
		string	redirect_url;
		string	root_index;

		std::map<string, conf::ServerLocation> location = config->get_locations();

		if (location.find(path) != location.end())
		{
			conf::ServerLocation::rules_map rules = location[path].get_rules();
			if (rules.find("return") != rules.end())
			{
				std::vector<string>::const_iterator return_it = rules["return"].begin();
				std::vector<string>::const_iterator return_end = rules["return"].end();

				for (; return_it != return_end; return_it++)
				{
					redirect_url = *return_it;
					cout << YELLOW << *return_it << RESET << endl;
				}
			}
			else if (rules.find("index") != rules.end())
			{
				std::vector<string>::const_iterator index_it = rules["index"].begin();
				std::vector<string>::const_iterator index_end = rules["index"].end();
				std::vector<string>::const_iterator root_it = rules["root"].begin();
				std::vector<string>::const_iterator root_end = rules["root"].end();
				string root, index;
				
				for (; root_it != root_end; root_it++)
					root = *root_it;
				for (; index_it != index_end; index_it++)
					index = *index_it;
				root_index = "." + root + "/" + index;
			}
			else if (rules.find("alias") != rules.end())
			{
				cout << "found" << endl;
				std::vector<string>::const_iterator alias_it = rules["alias"].begin();
				std::vector<string>::const_iterator alias_end = rules["alias"].end();

				for (; alias_it != alias_end; alias_it++)
				{
					redirect_url = *alias_it;
					cout << YELLOW << *alias_it << RESET << endl;
				}
			}
			else
				cout << "not found" << endl;

		}
		if (not redirect_url.empty())
			return this->handleGetResponse("", redirect_url);
		else if (not root_index.empty())
			return this->handleGetResponse(root_index, "");
		else
			return -1;

	}

	int Server::handleGetRequest()
	{
		string headers = HDE::Server::get_headers();
		string content = HDE::Server::get_content();
		string file = "./html/200.html", path, redirect_url;

		file = "404.html";
		path = headers.substr(headers.find("GET ") + 4);
		path = path.substr(0, path.find(" "));
		cout << YELLOW << path << RESET << endl;

		int ret_val;

		this->status = DONE;

		// -1 means it isnt a redirection 
		if ((ret_val = this->redirectClient(path)) != -1)
			return ret_val;

		path = "." + path;

		// jesus fucking christ bro what the fuck is this
		if (path == "./")
			file = "./html/" + config->get_index();
		else if (access(path.c_str(), R_OK) == 0)
			file = path;
		else if (path.find(".html") != string::npos)
			file = path;
		// else if (path.find("error.css") != string::npos)
		// 	file = "./html/component/error.css";
		else if (path.find(".py") != string::npos)
			file = path;

		if (file == "404.html")
			return this->sendError(file);

		cout << GREEN << "File: " << file << "	Path: " << path << RESET<< endl;


		if (file.find(".py") != string::npos)
		{
			return this->py();
		}
		return this->handleGetResponse(file, redirect_url);
	}

	// CGI METHODS

	// guys, chances are cgi method will not be in one limbillion gb righttt?? right??? dont need chunkin for this RIGHT???
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

	// ?????
	string Server::get_file_data(string filename)
	{
		string response;
		std::ifstream file;
		char img_buffer[BUFFER_SIZE];

		file.open(filename.c_str());
		if (file.is_open())
		{
			while (file.read(img_buffer, sizeof(img_buffer)))
				response.append(img_buffer, sizeof(img_buffer));
			if (file.eof())
				response.append(img_buffer, file.gcount());
			file.close();
		}
		else
			std::cerr << RED << "[ERROR] Error opening file." << endl;
		return (response);
	}

	int Server::py()
	{
		string exe_path = find_bin();	
		std::map<string, string> cgi_vec = config->get_cgi();

		int stdout_fd = dup(1), stdin_fd = dup(0);
		string content;

		int pipe_fd[2];
		if (pipe(pipe_fd) == -1)
		{
			std::cerr << RED << "[ERROR] Pipe failed" << std::endl;
			return 1;
		}

		int pid = fork();
		if (pid == 0)
		{
			string cgi_path = cgi_vec[".py"];

			dup2(pipe_fd[1], 1);
			close(pipe_fd[0]);

			std::vector<char *> env_vec;
			env_vec.push_back(strdup(string("CONTENT_TYPE=text/html").c_str()));
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
		{
			std::cerr << "fork failed" << std::endl;
			return 1;
		}
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
		header.append("Connection: keep-alive\r\n");
		// cout << RED << content << RESET << endl;
		sendData(this->newsocket, (void *)header.c_str(), header.size());
		sendData(this->newsocket, (void *)content.c_str(), content.size());
		return 0;
	}
}