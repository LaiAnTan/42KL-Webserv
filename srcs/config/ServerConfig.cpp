#include "Config.hpp"

namespace conf
{
	void ServerConfig::set_port(string text)
	{
		
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");
		
		if (tokens[0] != "listen")
			throw (conf::TooManyValuesException());

		tokens.erase(tokens.begin());

		this->port = tokens;
	}

	void	ServerConfig::set_root(string text)
	{
		// cout << "---root---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");
		
		if (tokens.size() != 2 || tokens[0] != "root")
			throw (conf::TooManyValuesException());

		this->root = tokens[1];
	}

	void	ServerConfig::set_index(string text)
	{
		// cout << "---index---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 2 || tokens[0] != "index")
			throw (conf::TooManyValuesException());

		this->index = tokens[1];
	}

	void	ServerConfig::set_server_name(string text)
	{
		// cout << "---server_name---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 2 || tokens[0] != "server_name")
			throw (conf::TooManyValuesException());

		this->index = tokens[1];
		// cout << "(" << this->server_name << ")" << endl;
	}

	void	ServerConfig::set_client_max(string text)
	{
		// cout << "---client_max---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 2 || tokens[0] != "client_max_body_size")
			throw (conf::TooManyValuesException());

		string res = tokens[1];

		string suffix = res.substr(res.size() - 2);

		if (not (suffix == "KB" || suffix == "MB" || suffix == "GB"))
		{
			if (res.substr(res.size() - 1) == "B")
				suffix = res.substr(res.size() - 1);
			else
				throw (conf::InvalidSuffixException());
		}

		char *end = NULL;

		cout << res.substr(0, res.find(suffix)).c_str() << endl;
		std::strtof(res.substr(0, res.find(suffix)).c_str(), &end);
		if (*end != '\0')
			throw (conf::InvalidValueException());

		this->client_max = res;
		// cout << "(" << this->client_max << ")" << endl;
	}

	void	ServerConfig::set_error(string text)
	{
		string	code;
		string error_path;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 3 || tokens[0] != "error_page")
			throw (conf::TooManyValuesException());

		code = tokens[1];
		error_path = tokens[2];

		this->error.insert(std::map<std::string, std::string>::value_type(code, error_path));
	}

	void	ServerConfig::set_cgi(string text)
	{
		string var1, var2;

		size_t cgiPos = text.find("cgi_script");
		string res = text.substr(cgiPos + 11);
		std::istringstream iss(res);
		iss >> var1 >> var2;
		this->cgi[var1] = var2;
	}

	void	ServerConfig::set_methods(string text)
	{
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() < 2 || tokens[0] != "allowed_methods")
			throw (conf::TooManyValuesException());

		tokens.erase(tokens.begin());
		this->allowed_method = tokens;
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

	const std::map<string, string> &ServerConfig::get_error() const
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
		int		i;
		size_t	semicolon_pos;
		size_t	comment_pos;
		string	text;
		void	(ServerConfig::*funct[])(string text) = {&conf::ServerConfig::set_port, \
			&conf::ServerConfig::set_root, &conf::ServerConfig::set_index, \
			&conf::ServerConfig::set_server_name, &conf::ServerConfig::set_client_max, \
			&conf::ServerConfig::set_error, &conf::ServerConfig::set_cgi, &conf::ServerConfig::set_methods};
		string arr[] = {"listen", "root", "index", "server_name", \
			"client_max_body_size", "error_page", "cgi_script", "allowed_methods"};

		while (std::getline(*file, text))
		{
			if (text[0] == '}')
				break;
			if ((comment_pos = text.find('#')) != std::string::npos) // remove comments
				text.resize(comment_pos);
			
			if (text.find_first_not_of(" \t\n\v\f\r") == std::string::npos) // handle blank lines
				continue;
			i = 0;
			while (i < 8 && text.find(arr[i]) == std::string::npos)
				i++;
			if (i < 8)
			{
				if ((semicolon_pos = text.find(";")) == string::npos)
					throw (conf::MissingSemicolonException());
				text.resize(semicolon_pos);
				(this->*funct[i])(text);
			}
			else if (text.find("location /") != std::string::npos)
				location_name(text, file);
			else if (text.empty() == false)
				throw (conf::InvalidKeywordException());
		}
		if (this->server_name.empty())
			this->server_name = "localhost";
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

		std::map<string, string>::iterator error_it;
		std::map<string, string> err = server_config.get_error(); 
		std::vector<string>::iterator htmls_it;
		for (error_it = err.begin(); error_it != err.end(); error_it++)
		{
			outs << YELLOW "Error : " RESET << error_it->first << "	" << MAGENTA << "Page : " << RESET << error_it->second << endl;
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