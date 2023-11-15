#include "Config.hpp"

namespace conf
{
	void ServerConfig::set_port(string text)
	{
		string res, word;

		size_t listenPos = text.find("listen");
    	size_t semicolonPos = text.find(";", listenPos);
		res = text.substr(listenPos + 7, semicolonPos - listenPos - 7);
		std::stringstream ss(res);
		while (std::getline(ss, word, ' '))
			this->port.push_back(word);
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
		string var1, var2, var3, key;
		std::vector<string> error_html;

		size_t errorPos = text.find("error_page");
    	size_t semicolonPos = text.find(";", errorPos);
		string res = text.substr(errorPos + 11, semicolonPos - errorPos - 11);
		std::stringstream ss(res);
		while (std::getline(ss, var1, '	'))
		{
			if (var1.empty())
				continue;
			else
				error_html.push_back(var1);
		}
		key = error_html.front();
		error_html.erase(error_html.begin());
		std::stringstream err(error_html.front());
		error_html.clear();
		while(std::getline(err, var2, ' '))
		{
			if (var2.empty())
				continue;
			else
				error_html.push_back(var2);
		}
		this->error[key] = error_html;
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

	void	ServerConfig::set_methods(string text)
	{
		string	var1;
		size_t methods = text.find("allowed_methods");
    	size_t semicolonPos = text.find(";", methods);
		string res = text.substr(methods + 16, semicolonPos - methods - 16);
		std::stringstream ss(res);
		while (std::getline(ss, var1, ' '))
		{
			if (var1.empty())
				continue;
			else
				this->allowed_method.push_back(var1);
		}
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

	const std::vector<string>	&ServerConfig::get_port() const
	{
		return(this->port);
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

	const std::map<string, std::vector<string> > &ServerConfig::get_error() const
	{
		return(this->error);
	}

	const std::map<string, string>	&ServerConfig::get_cgi() const
	{
		return(this->cgi);
	}

	const std::vector<string> &ServerConfig::get_methods() const
	{
		return(this->allowed_method);
	}

	const std::map<string, ServerLocation>	&ServerConfig::get_locations() const
	{
		return(this->locations);
	}

	ServerConfig::ServerConfig(std::ifstream *file)
	{
		string text;
		int i;
		void (ServerConfig::*funct[])(string text) = {&conf::ServerConfig::set_port, \
			&conf::ServerConfig::set_root, &conf::ServerConfig::set_index, \
			&conf::ServerConfig::set_server_name, &conf::ServerConfig::set_client_max, \
			&conf::ServerConfig::set_error, &conf::ServerConfig::set_cgi, &conf::ServerConfig::set_methods};
		string arr[] = {"listen", "root", "index", "server_name", \
			"client_max_body_size", "error_page", "cgi_script", "allowed_methods"};

		while (std::getline(*file, text))
		{
			if (text[0] == '}')
				break;
			if (text.find('#') == std::string::npos)
			{
				i = 0;
				while (i < 8 && text.find(arr[i]) == std::string::npos)
					i++;
				if (i < 8)
					(this->*funct[i])(text);
				else if (text.find("location /") != std::string::npos)
					location_name(text, file);
				else if (text.empty() == false)
					throw (conf::InvalidKeywordException());
				if (this->server_name.empty())
					this->server_name = "localhost";
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
			this->port = server_config.port;
			this->root = server_config.root;
			this->index = server_config.index;
			this->server_name = server_config.server_name;
			this->client_max = server_config.client_max;
			this->error = server_config.error;
			this->cgi = server_config.cgi;
			this->allowed_method = server_config.allowed_method;
			this->locations = server_config.locations;
		}
		return (*this);
	}

	ServerConfig::~ServerConfig()
	{}

	std::ostream &operator << (std::ostream &outs, const ServerConfig &server_config)
	{
		std::vector<string>				ports = server_config.get_port();
		ServerConfig::portIter			it, end = ports.end();

		outs << YELLOW "Server Name : " RESET << server_config.get_server_name() << endl;

		for (it = ports.begin(); it != end; ++it)
			outs << YELLOW "Port : " RESET << *it << endl;
		
		outs << YELLOW "Root : " RESET << server_config.get_root() << endl;
		outs << YELLOW "Index : " RESET << server_config.get_index() << endl;
		outs << YELLOW "Client_max : " RESET << server_config.get_client_max() << endl;

		std::map<string, std::vector<string> >::iterator error_it;
		std::map<string, std::vector<string> > err = server_config.get_error(); 
		std::vector<string>::iterator htmls_it;
		for (error_it = err.begin(); error_it != err.end(); error_it++)
		{
			outs << YELLOW "Error : " RESET << error_it->first << "	" << MAGENTA "Page :" RESET;
			std::vector<string>	htmls = error_it->second;
			for (htmls_it = htmls.begin(); htmls_it != htmls.end(); htmls_it++)
				outs << " " << *htmls_it;
			outs << endl;
		}
		std::map<string, string>::iterator map_it;
		std::map<string, string> tmp = server_config.get_cgi(); 
		for (map_it = tmp.begin(); map_it != tmp.end(); map_it++)
			outs << YELLOW "CGI : " RESET << map_it->first << "   " << map_it->second << endl;

		std::vector<string>::iterator	methods_it;
		std::vector<string>	methods = server_config.get_methods();
		outs << YELLOW "Allowed_methods : " RESET;
		for (methods_it = methods.begin(); methods_it != methods.end(); methods_it++)
			outs << *methods_it << " ";
		outs << endl;
	
		outs << endl;
		ServerConfig::locationMap::iterator location_it;
		ServerConfig::locationMap location_map = server_config.get_locations();

		for (location_it = location_map.begin(); location_it != location_map.end(); location_it++)
		{
			outs << BLACK "Location : " RESET << location_it->first << endl;
			outs << (*location_it).second << endl;
		}

		return (outs);
	}
}