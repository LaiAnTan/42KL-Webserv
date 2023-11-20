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

		// cout << YELLOW << "[INFO] Sending Following Content\n" << chunk_segment.str() << RESET << endl; 

		if (sending.length() == 0)
			this->status = DONE;
		else
			this->status = SEND_CHUNK;
		return this->sendData(this->newsocket, chunk_segment.str().c_str(), chunk_segment.str().length());
	}

	int Server::handleGetResponse()
	{
		string				header, extension, mime_type;
		std::stringstream	response;
		int		ret_val;

		this->status = DONE;

		// check if it is a python file (check extension) (epic cgi handlin)

		if (real_filepath.find(".py") != string::npos){
			return this->py();
		}

		// for redirects
		if (not redirect_url.empty())
		{
			cout << "Redirection Url = " << redirect_url << endl;
			string response;
			response.append("HTTP/1.1 302 Found\r\n");
			response.append("Location:" + redirect_url + "\r\n\r\n");
			return sendData(this->newsocket, (void *)response.c_str(), response.size());
		}

		// handles header
		header.append("HTTP/1.1 200 OK\r\n");
		header.append("Connection: keep-alive\r\n");

		// for send file contents
		if (real_filepath.find(".", 1) != string::npos)
			extension = real_filepath.substr(real_filepath.find(".", 1));
		else
			extension = "";
		// https://stackoverflow.com/questions/1176022/unknown-file-type-mime
		mime_type = this->get_type(extension);
		if (mime_type != ""){
			header.append("Content-Type: ");
			header.append(mime_type);
			header.append("\r\n");
		}

		// determine if chunkey is needed
		struct stat st;
		stat(real_filepath.c_str(), &st);
		int size = st.st_size;

		// size o file is less than buffer size, can just straight read
		// everything
		if (size < BUFFER_SIZE)
		{
			cout << YELLOW << "SENDING THE ENTIRE FILE AS ONE" << RESET << endl;

			response << header;
			response << "Content-Length: " << size << "\r\n";
			response << "\r\n";
			response << get_file_data(real_filepath.c_str());

			// cout << YELLOW << "response sended: \n" << response.str() << endl;

			ret_val = sendData(this->newsocket, (void *)response.str().c_str(), response.str().size());
			if (ret_val < 0)
				std::cerr << RED << "Error sending file " << real_filepath << endl;
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
			this->file.open(real_filepath.c_str());
			if (ret_val < 0)
			{
				std::cerr << RED << "Error sending header for chunking" << endl;
				return 1;
			}
			return this->send_next_chunk();
		}
	}

	int Server::sendError(string error_code)
	{
		this->status = DONE;

		string				code[] = {"400", "404", "405", "413", "500", "501", "505"};
		string				msg[] = {"Bad Request", "Not Found", "Method Not Allowed", "Payload Too Large", "Internal Server Error", "Not Implemented", "HTTP Version Not Supported"};

		std::ifstream					file;
		string							filename, error_content, error_description;
		bool							found;
		const std::map<string, string>	error_map = config->get_error();

		for (int i = 0; i < 7; i++)
		{
			if (error_code.find(code[i]) != string::npos){
				error_description = msg[i];
				found = true;
				break;
			}
		}

		if(!error_map.empty() && error_map.find(error_code) != error_map.end())
		{
			filename = error_map.at(error_code);
			file.open(filename.c_str());
			if (file.is_open())
				error_content.append(get_file_data(filename));
			else
				cout << RED << "[ERROR] Config Error - File cannot be opened" << endl;
		}

		// default error html
		// send this if there is no specific html stated in config
		string				header;
		std::stringstream	response;
		string				find_code = "[CODE]";
		string				find_msg = "[MSG]";

		if (!file.is_open())
		{
			cout << YELLOW << "[INFO] Opening default error file" << endl;

			if (not found)
			{
				cout << RED << "[ERROR] Invalid Code" << endl;
				// mhm, no idea what to do if invalid code
				error_content.append(get_file_data("./default_error/bad_error.html"));
				error_content.replace(error_content.find("[CODE]"), 6, error_code);
			}
			else
			{
				file.open("./default_error/error.html");
				// get content
				// and substitute the CODE and MSG in the error.html (why is this a thing?)
				if (file.is_open())
				{
					while (!file.eof())
					{
						string html;
						std::getline(file, html);
						if (html.find(find_code) != string::npos)
							html.replace(html.find(find_code), find_code.length(), error_code);
						if (html.find(find_msg) != string::npos)
							html.replace(html.find(find_msg), find_msg.length(), error_description);
						error_content.append(html);
					}
					file.close();
				}
				else
					std::cerr << "[ERROR] Oh great, even the default one cant open" << endl;
			}
		}
		else
			file.close();
		// handles header
		response << "HTTP/1.1 " << error_code << " " << error_description << "\r\n";
		response << "Connection: keep-alive\r\n";
		response << "Content-Type: text/html\r\n";
		response << "Content-Length: " << error_content.length() << "\r\n";
		response << "\r\n";
		response << error_content;

		return sendData(this->newsocket, (void *)response.str().c_str(), response.str().size());
	}

	int Server::redirectClient()
	{
		string									redirect_url, root_index;
		std::map<string, conf::ServerLocation>	location = config->get_locations();

		if (location.find(this->location_config_path) == location.end())
			return (0);

		redirect_url = location[this->location_config_path].get_return_path();
		if (redirect_url.empty() == true)
			return (-1);
		
		this->redirect_url = redirect_url;
		return 0;
	}

	string	Server::config_path()
	{
		string										root_index, root = "", index = "", alias = "";
		std::map<string, conf::ServerLocation>		location = config->get_locations();
		bool										need_gen_index;

		// end, copy this over thanks :P

		if (location.find(this->location_config_path) != location.end())
		{
			// root
			root = location[this->location_config_path].get_root();
			if (root.empty() == true)
				root = config->get_root(); // use default Server root

			// index file
			index = location[this->location_config_path].get_index();
			if (index.empty() == true)
			{
				string	autoindex_value = location[this->location_config_path].get_autoindex();
				if (autoindex_value == "on")
				{
					index = "";
					need_gen_index = true;
				}
				else
					index = "index.html";
			}

			// alias

			// Placing a alias directive in a location block 
			// overrides the root or alias directive that was 
			// applied at a higher scope.
			// alias = location[location_path].get_alias();
			// if (alias.empty() == false)
			// {
			// 	if (root.empty() == false)
			// 	{
			// 		cout << RED << "[Warning] Both Alias and Root found";
			// 	}
			// }

			// so uhh, the pdf actually treats root as alias
			// what the fuck
		}
		// not found, use "/"
		else
		{
			root = config->get_root();
			index = "index.html";
		}

		string	new_path = this->path.substr(this->location_config_path.length());
		if (new_path[0] == '/' || root[root.length() - 1] == '/')
			root_index = root + new_path;
		else
			root_index = root + "/" + new_path;

		bool	is_folder = false;

		if (root_index[root_index.length() - 1] == '/'){
			is_folder = true;
		}

		struct stat path_stat;
		if (!stat(root_index.c_str(), &path_stat) && !S_ISREG(path_stat.st_mode)){
			is_folder = true;
		}

		if (is_folder)
		{
			this->auto_index = need_gen_index;
			if (root_index[root_index.length() - 1] != '/')
				root_index = root_index + "/";
			if (not this->auto_index)
			{
				if (index[0] == '/')
					root_index = root_index + index.substr(1);
				else
					root_index = root_index + index;
			}
		}
		cout << "Path To File: " << root_index << endl;
		return root_index;
	}

	int Server::handleGetRequest()
	{
		string headers = HDE::Server::get_headers();
		string content = HDE::Server::get_content();
		string path, redirect_url;

		path = headers.substr(headers.find("GET ") + 4);
		path = path.substr(0, path.find(" "));
		cout << YELLOW << path << RESET << endl;

		// -1 means it isnt a redirection 
		if (this->redirectClient() != -1)
			return ACP_FINISH;

		// build path based on config file
		path = config_path();
		this->real_filepath = path;

		if (this->auto_index == true)
		{
			this->status = CLEARING_SOCKET;
			return ACP_FINISH;
		}

		// it is a python file, time for cgi
		if (path.find(".py") != string::npos)
			return ACP_FINISH;

		cout << GREEN << "Path: " << path << RESET << endl;

		struct stat stats;
		// check if it exist			// check if it is a regular file
		if (stat(path.c_str(), &stats) || !S_ISREG(stats.st_mode))
			this->error_code = "404";
		return ACP_FINISH;
	}

	// CGI METHODS
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

	std::vector<std::pair<string, string> >	Server::extract_cgi_variable(const string &source)
	// dont need map since we will not be accessing any values
	// please make sure you give it the values AFTER the ? mark
	{
		std::vector<std::pair<string, string> >	ret_array;
		std::vector<string>						variables = util::split(source, "&");
		std::vector<string>::iterator			var_start;
		std::vector<string>						kw_pair;

		for (var_start = variables.begin(); 
			 var_start != variables.end(); 
			 ++var_start)
		{
			std::pair<string, string>	new_pair; // im telling you this is probably illegal
			kw_pair = util::split(*(var_start), "=");

			for (std::vector<string>::iterator	kw_start = kw_pair.begin();
				kw_start != kw_pair.end();
				++kw_start)
			{
				while (kw_start->find('+') != string::npos)
					kw_start->replace(kw_start->find('+'), 1, " ");
			}

			new_pair.first = decode_data(kw_pair[0]);
			new_pair.second = decode_data(kw_pair[1]);
			ret_array.push_back(new_pair);
		}
		return ret_array;
	}

	int Server::py()
	{
		string													exe_path = find_bin(), cgi_path;	
		std::vector<std::pair<string, string> >					kw_var;
		std::vector<std::pair<string, string> >::iterator		kw_var_start;
		int stdout_fd = dup(1), stdin_fd = dup(0);

		string header_send, content_send;

		int pipe_fd[2];
		if (pipe(pipe_fd) == -1)
		{
			std::cerr << RED << "[ERROR] Pipe failed" << std::endl;
			return 1;
		}

		// get keyword arguments for cgi
		if (this->path.find("?") != string::npos)
			kw_var = extract_cgi_variable(this->path.substr(this->path.find('?') + 1));

		cout << CYAN << "[INFO] -- Keyword Argument Extracted -- " << endl;
		for (kw_var_start = kw_var.begin(); kw_var_start != kw_var.end(); ++kw_var_start)
			cout << "Keyword = " << kw_var_start->first << "  " << "Argument = " << kw_var_start->second << endl;
		cout << RESET;

		int pid = fork();
		if (pid == 0)
		{
			// seperate keyword arg
			if (this->real_filepath.find("?") != string::npos)
				cgi_path = this->real_filepath.substr(0, this->real_filepath.find('?'));
			else
				cgi_path = this->real_filepath;

			dup2(pipe_fd[1], 1);
			close(pipe_fd[0]);

			std::vector<char *> env_vec;
			// mm should this be hardcoded...?
			env_vec.push_back(strdup(string("CONTENT_TYPE=text/html").c_str()));

			string	new_kw_arg;
			for (kw_var_start = kw_var.begin(); kw_var_start != kw_var.end(); ++kw_var_start)
			{
				new_kw_arg = kw_var_start->first + "=" + kw_var_start->second;
				env_vec.push_back(strdup(string(new_kw_arg).c_str()));
				new_kw_arg.clear();
			}
			env_vec.push_back(NULL);

			char *args[] = {const_cast<char *>(exe_path.c_str()), const_cast<char *>(cgi_path.c_str()), NULL};
			execve(exe_path.c_str(), args, env_vec.data());
			std::cerr << "execve failed" << std::endl;
			exit(1);
		}
		else if (pid < 0)
		{
			std::cerr << "fork failed" << std::endl;
			return RES_ERROR;
		}
		else
		{
			close(pipe_fd[1]);
			waitpid(pid, NULL, 0);
			char buffer[BUFFER_SIZE];
			ssize_t bytes_read;
			while ((bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE)) > 0)
				content_send.append(buffer, bytes_read);
			close(pipe_fd[0]);
		}
		dup2(stdout_fd, 1);
		dup2(stdin_fd, 0);

		header_send.append("HTTP/1.1 200 OK\r\n");
		header_send.append("Connection: keep-alive\r\n");
		

		if (sendData(this->newsocket, (void *)header_send.c_str(), header_send.size()) == RES_ERROR
			|| sendData(this->newsocket, (void *)content_send.c_str(), content_send.size()) == RES_ERROR)
			return RES_ERROR;
		return RES_SUCCESS;
	}
}