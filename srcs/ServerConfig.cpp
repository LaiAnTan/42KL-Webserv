#include "webserv.hpp"

namespace conf
{
	void ServerConfig::set_listen(string text)
	{
		string res, word;

		size_t listenPos = text.find("listen");
    	size_t semicolonPos = text.find(";", listenPos);
		res = text.substr(listenPos + 7, semicolonPos - listenPos - 7);
		std::stringstream ss(res);
		while (std::getline(ss, word, ' '))
			this->listen.push_back(word);
	}

	void	ServerConfig::set_root(string text)
	{
		// cout << "---root---" << endl;
		size_t rootPos = text.find("root");
    	size_t semicolonPos = text.find(";", rootPos);
		string res = text.substr(rootPos + 5, semicolonPos - rootPos - 5);
		this->root = res;
		// cout << "(" << this->root << ")" << endl;
	}

	void	ServerConfig::set_index(string text)
	{
		// cout << "---index---" << endl;
		size_t indexPos = text.find("index");
    	size_t semicolonPos = text.find(";", indexPos);
		string res = text.substr(indexPos + 6, semicolonPos - indexPos - 6);
		this->index = res;
		// cout << "(" << this->index << ")" << endl;
	}

	void	ServerConfig::set_server_name(string text)
	{
		// cout << "---server_name---" << endl;
		size_t serverPos = text.find("server_name");
    	size_t semicolonPos = text.find(";", serverPos);
		string res = text.substr(serverPos + 12, semicolonPos - serverPos - 12);
		this->server_name = res;
		// cout << "(" << this->server_name << ")" << endl;
	}

	void	ServerConfig::set_client_max(string text)
	{
		// cout << "---client_max---" << endl;
		size_t clientPos = text.find("client_max_body_size");
    	size_t semicolonPos = text.find(";", clientPos);
		string res = text.substr(clientPos + 21, semicolonPos - clientPos - 21);
		this->client_max = res;
		// cout << "(" << this->client_max << ")" << endl;
	}

	void	ServerConfig::set_error(string text)
	{
		string var1, var2;

		size_t errorPos = text.find("error_page");
    	size_t semicolonPos = text.find(";", errorPos);
		string res = text.substr(errorPos + 11, semicolonPos - errorPos - 11);
		std::istringstream iss(res);
		iss >> var1 >> var2;
		this->error[var1] = var2;
	}

	void	ServerConfig::set_cgi(string text)
	{
		string var1, var2;

		size_t cgiPos = text.find("cgi_script");
    	size_t semicolonPos = text.find(";", cgiPos);
		string res = text.substr(cgiPos + 11, semicolonPos - cgiPos - 11);
		std::istringstream iss(res);
		iss >> var1 >> var2;
		this->cgi[var1] = var2;
	}
	
	void	ServerConfig::location_name(string text, std::ifstream *file)
	{
		string var1, var2;
		std::istringstream iss(text);

		iss >> var1 >> var2;
		// cout << "var1: " << var1 << endl;
		cout << "location: " << var2 << endl;
		// ServerLocation	tmp(file);
		// this->locations[var2] = tmp;
		this->locations.insert(std::make_pair(var2, ServerLocation(file)));
		// this->locations[var2] = tmp;

	}

	ServerConfig::ServerConfig(std::ifstream *file, int start, int end)
	{
		string text;
		int i;
		void (ServerConfig::*funct[])(string text) = {&conf::ServerConfig::set_listen, \
			&conf::ServerConfig::set_root, &conf::ServerConfig::set_index, \
			&conf::ServerConfig::set_server_name, &conf::ServerConfig::set_client_max, \
			&conf::ServerConfig::set_error, &conf::ServerConfig::set_cgi};
		string arr[] = {"listen	", "root	", "index	", "server_name	", \
			"client_max_body_size	", "error_page	", "cgi_script	"};
		while (std::getline(*file, text))
		{
			if (text.find('#') == std::string::npos)
			{
				i = 0;
				while (i < 6 && text.find(arr[i]) == std::string::npos)
					i++;
				if (i < 6)
					(this->*funct[i])(text);
				if (this->server_name.empty())
					this->server_name = "localhost";
				if (text.find("location /") != std::string::npos)
				{
					location_name(text, file);
				}
			}
			if (text[0] == '}')
			{
				//testing -->
				for (size_t i = 0; i < listen.size(); ++i)
					std::cout << "port " << listen[i] << std::endl;
				cout << this->root << endl;
				cout << this->index << endl;
				cout << this->server_name << endl;
				cout << this->client_max << endl;
				std::map<string, string>::iterator it;
				for (it = error.begin(); it != error.end(); it++)
					cout << it->first << " , " << it->second << endl;
				for (it = cgi.begin(); it != cgi.end(); it++)
					cout << it->first << " , " << it->second << endl;
				// <--
				break;
			}
		}

	}

	ServerConfig::ServerConfig(const ServerConfig &server_config)
	{
		*this = server_config;
	}

	ServerConfig &ServerConfig::operator=(const ServerConfig &server_config)
	{
		if (this != &server_config)
		{

		}
		return (*this);
	}

	ServerConfig::~ServerConfig()
	{

	}
}