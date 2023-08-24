#include "webserv.hpp"

namespace conf
{
	ServerConfig::ServerConfig(std::ifstream *file, int start, int end)
	{
		string text;
		int i;
		void (ServerConfig::*funct[])(string text) = {
			&conf::ServerConfig::set_ports,		\
			&conf::ServerConfig::set_root,			\
			&conf::ServerConfig::set_index,			\
			&conf::ServerConfig::set_server_name,	\
			&conf::ServerConfig::set_client_max,	\
			&conf::ServerConfig::set_error, 		\
			&conf::ServerConfig::set_cgi			\
			};
		string arr[] = {
			"listen",\
			"root",\
			"index",\
			"server_name",\
			"client_max_body_size",\
			"error_page",\
			"cgi_script"\
			};
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
				this->root = "test";
				//testing -->
				// cout << *this << endl;
				// std::map<string, string>::iterator it;
				// for (it = error.begin(); it != error.end(); it++)
				// 	cout << it->first << " , " << it->second << endl;
				// for (it = cgi.begin(); it != cgi.end(); it++)
				// 	cout << it->first << " , " << it->second << endl;
				// <--
				break;
			}
		}

	}

	ServerConfig::ServerConfig(const ServerConfig &server_config)
	{
		*this = server_config;
	}

	ServerConfig &ServerConfig::operator = (const ServerConfig &server_config)
	{
		if (this != &server_config)
		{
			this->ports = server_config.ports;
			this->root = server_config.root;
			this->index = server_config.index;
			this->server_name = server_config.server_name;
			this->client_max = server_config.client_max;
			this->error = server_config.error;
			this->cgi = server_config.cgi;
			this->locations = server_config.locations;
		}
		return (*this);
	}

	ServerConfig::~ServerConfig()
	{
		cout << "SeverConfig: Destructor called" << endl;
	}
	
	void ServerConfig::set_ports(string text)
	{
		string res, word;

		size_t listenPos = text.find("listen");
		size_t semicolonPos = text.find(";", listenPos);
		res = text.substr(listenPos + 7, semicolonPos - listenPos - 7);
		std::stringstream ss(res);
		while (std::getline(ss, word, ' '))
			this->ports.push_back(word);
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
		// cout << "location: " << var2 << endl;
		// ServerLocation	tmp(file);
		// this->locations[var2] = tmp;
		this->locations.insert(std::make_pair(var2, ServerLocation(file)));
	}

	string	ServerConfig::get_root() const
	{
		return (this->root);
	}

	string	ServerConfig::get_index() const
	{
		return (this->index);
	}

	string	ServerConfig::get_server_name() const
	{
		return (this->server_name);
	}

	string	ServerConfig::get_client_max() const
	{
		return (this->client_max);
	}

	const std::vector<string>	&ServerConfig::get_ports() const
	{
		return (this->ports);
	}

	const std::map<string, string>	&ServerConfig::get_error() const
	{
		return (this->error);
	}

	const std::map<string, string>	&ServerConfig::get_cgi() const
	{
		return (this->cgi);
	}

	const ServerConfig::locationMap	&ServerConfig::get_locations() const
	{
		return (this->locations);
	}

	std::ostream &operator << (std::ostream &outs, const ServerConfig &server_config)
	{
		std::vector<string>				ports = server_config.get_ports();
		ServerConfig::portIter			it = ports.begin();
		ServerConfig::portIter			end = ports.end();

		outs << "Server Name : " << server_config.get_server_name() << endl;

		for (it; it != end; ++it)
			outs << "Port : " << *it << endl;
		
		outs << "Root : " << server_config.get_root() << endl;
		outs << "Index : " << server_config.get_index() << endl;
		outs << "Client_max : " << server_config.get_client_max() << endl;

		return (outs);
	}
}
