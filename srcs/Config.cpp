#include "webserv.hpp"



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
			// this->servers = config.servers;
		}
		return (*this);
	}

	Config::~Config()
	{}

	void	Config::config_handle(std::ifstream *file)
	{
		string text;
		int loop = 0, start = 0, end = 0;
		int c = 0;
		ServerConfig *tmp;

		while (std::getline(*file, text))
		{
			loop++;
			if (text.find("server {") < text.length() || text.find("server	{") < text.length())
			{
				start = loop;
				this->servers.push_back(ServerConfig(file, start, end));
				c++;
			}
			// if (text[0] == '}')
			// {
			// 	end = loop;
			// 	cout << start << "     " << end << endl;
			// }

			// if (text.find('#') > text.length())
			// {
			// 	std::stringstream ss(text);
			// 	while (std::getline(ss, word, '	'))
			// 	{
			// 		cout << word << "-";
			// 	}
			// 	cout << endl;
			// }
		}
		cout << "server count: " << c << endl;
		for (size_t i = 0; i < servers.size(); ++i)
		{
			std::vector<string> list = servers[i].get_listen();
			for (size_t a = 0; a < list.size(); ++a)
					cout << "port : " << list[a] << endl;
			cout << "server : " << i << endl;
			cout << "root : " << servers[i].get_root() << endl;
			cout << "index : " << servers[i].get_index() << endl;
			cout << "server_name : " << servers[i].get_server_name() << endl;
			cout << "client_max : " << servers[i].get_client_max() << endl;
			cout << endl << endl;
		}
	}
}