#include "Server.hpp"
#include "../../srcs/Utils.hpp"

# include <stdio.h>
# include <stdlib.h>
# include <dirent.h>

using std::cout;
using std::endl;
using std::cerr;

namespace HDE
{

	void remove_directory(std::string username)
	{
		int	ret_val;
		std::string path = "./Users/";
		path.append(username);
		DIR *dir = opendir(path.c_str());

		if (!dir)
		{
			perror("opendir");
			return ;
		}
		struct dirent * dr;

		std::string file_name;
		std::string file_path;
		while ((dr = readdir(dir)))
		{
			file_name = dr->d_name;

			// dont delete the file
			if (file_name == username)
				continue;
			// dont delete ..
			else if (file_name == "..")
				continue;
			// dont delete .
			else if (file_name == ".")
				continue;

			file_name.insert(file_name.cbegin() ,'/');
			file_path.clear();
			file_path.append(path);
			file_path.append(file_name);

			cout << YELLOW << "[INFO] Removing file - " << file_path.c_str() << RESET << endl;
			// uh oh, we cant use remove :(((((
			ret_val = remove(file_path.c_str());
			if (ret_val)
				cout << RED << "[ERROR] " << strerror(errno) << RESET << endl;
			file_name.clear();
		}
		closedir(dir);
		// remove the directory
		rmdir(path.c_str());

		return ;
	}

	void	Server::handleDeleteRequest()
	{
		std::string 		filename;
		std::string			header = get_headers();
		std::vector<string>	header_tokens = util::split(header, string(" "));

		if (header_tokens.empty() == true || header_tokens[0] != "DELETE")
			return ;
		
		// filename = header_tokens[1];

		// i nead the username and password here!!!!!!
		// remove slash in front of username
		std::string username = header_tokens[1];
		username.erase(username.cbegin());
		cout << username << endl;

		remove_directory(username);
	}

	void	Server::createDeleteResponse(int socket, string content, string content_type, bool is_deleted)
	{
		string	response; // response string to be sent to client

		/*
		codes:
		- 202 Accepted- accepted but havent delete
		- 200 OK - accepted and deleted, response contains content
		- 204 No Content - accepted and deleted, response dont have content
		(stay on same page)
		*/ 

		if (is_deleted == false)
			response.append("HTTP/1.1 202 Accepted\r\n");
		else
		{
			if (content.empty() == true)
				response.append("HTTP/1.1 204 No Content\r\n");
			else
			{
				response.append("HTTP/1.1 200 OK\r\n");
				
				// append content type & content here
			}
			return ;
		}
	}
}