#include "Config.hpp"

namespace conf
{
	Config::Config()
	{}

	Config::Config(std::string filename)
	{
		std::ifstream config_filestream(filename.c_str());
		if (!config_filestream.fail())
		{
			config_handle(&config_filestream);
			config_filestream.close();
		}
		else
			throw conf::Error(RED "File not found" RESET);
	}

	Config::Config(const Config &config)
	{
		*this = config;
	}

	Config	&Config::operator=(const Config &config)
	{
		if (this != &config)
		{
			this->servers = config.servers;
		}
		return (*this);
	}

	Config::~Config()
	{}

	void	Config::config_handle(std::ifstream *file)
	{
		string text;
		int c = 0;

		while (std::getline(*file, text))
		{
			if (text.find("server {") < text.length() || text.find("server	{") < text.length())
			{
				this->servers.push_back(ServerConfig(file));
				c++;
			}
		}
	}

	const std::vector<ServerConfig> &Config::get_servers() const
	{
		return (this->servers);
	}

	std::ostream &operator << (std::ostream &outs, const Config &config)
	{
		cout << GREEN "Server count : " RESET << config.get_servers().size() << endl << endl;
		std::vector<ServerConfig>::const_iterator	it, end = config.get_servers().end();

		for (it = config.get_servers().begin(); it != end; ++it)
			outs << *it << endl;
		return (outs);
	}

	const char* conf::InvalidKeywordException::what() const throw()
	{
		return ("InvalidKeywordException: Invalid Keyword found in config file");
	}

	const char* conf::MissingSemicolonException::what() const throw()
	{
		return ("MissingSemicolonException: Missing Semicolon in config file");
	}
}