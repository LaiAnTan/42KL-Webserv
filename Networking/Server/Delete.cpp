#include "Server.hpp"
#include "Utils.hpp"
#include <sys/stat.h>
#include <cstdio>

using std::cout;
using std::endl;
using std::cerr;

namespace HDE
{
	bool file_exists(std::string path)
	{
		struct stat buffer;

		return (stat(path.c_str(), &buffer) == 0);
	}

	void	Server::handleDeleteRequest(int socket)
	{
		std::string			root;
		std::string			path;
		std::string			header = get_headers();
		std::vector<string>	header_tokens = util::split(header, string(" "));

		cout << header << endl;

		if (header_tokens.empty() == true || header_tokens[0] != "DELETE")
			return ;

		// i hate hardcoding ???
		root = "root";

		path = root + header_tokens[1];
		if (file_exists(path))
		{
			cout << "File exists" << endl;
			// do delete
			std::remove(path.c_str());
		}

		if (!file_exists(path))
			createDeleteResponse(socket, true);
		else
			createDeleteResponse(socket, false);
		return ;
	}

	void	Server::createDeleteResponse(int socket, bool is_deleted)
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
			response.append("HTTP/1.1 200 OK\r\n");

		sendData(socket, (void *) response.c_str(), response.size());
		return ;
	}
}