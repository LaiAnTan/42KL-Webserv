#include "Server.hpp"
#include "../config/Config.hpp"
#include "../util/Utils.hpp"
#include <sys/stat.h>
#include <cstdio>

using std::cout;
using std::endl;
using std::cerr;

namespace HDE
{
	bool Server::file_exists(std::string path)
	{
		struct stat buffer;

		return (stat(path.c_str(), &buffer) == 0);
	}

	int	Server::handleDeleteRequest()
	{
		std::string			root;
		std::string			header = get_headers();
		std::vector<string>	header_tokens = util::split(header, string(" "));
		std::map<string, conf::ServerLocation>		location = config->get_locations();

		// simpler version of get's file idk blabla
		if (location.find(this->location_config_path) != location.end())
		{
			// root
			root = location[this->location_config_path].get_root();
			if (root.empty() == true)
				root = config->get_root(); // use default Server root
		}
		else
			root = config->get_root();

		string	new_path = this->path.substr(this->location_config_path.length());
		if (new_path[0] == '/' || root[root.length() - 1] == '/')
			this->real_filepath = root + new_path;
		else
			this->real_filepath = root + "/" + new_path;
		// simpler version of get's file end

		cout << "File to delete: " << this->real_filepath << endl;

		if (file_exists(this->real_filepath))
		{
			cout << "File exists" << endl;
			// do delete
			std::remove(this->real_filepath.c_str());
		}

		this->is_deleted = !file_exists(this->real_filepath);
		return 0;
	}

	int	Server::handleDeleteResponse()
	{
		string	response; // response string to be sent to client
		/*
		codes:
		- 202 Accepted- accepted but havent delete
		- 200 OK - accepted and deleted, response contains content
		- 204 No Content - accepted and deleted, response dont have content
		(stay on same page)
		*/
		this->status = DONE;

		if (is_deleted == false)
			response.append("HTTP/1.1 202 Accepted\r\n");
		else
			response.append("HTTP/1.1 204 No Content\r\n");
		response.append("\r\n");

		cout << "Header Sent: \n" << response << endl;
		return sendData(this->newsocket, (void *) response.c_str(), response.size());
	}
}