#pragma once

#ifndef CONFIG_HPP

#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

using std::string;

class ServerConfig
{
	public:



	private:
		std::vector<string> listen;

		std::map<string, std::vector<string> >	locations;

};

class Config
{
	public:
		Config();
		Config(std::string filename);
		Config(const Config &config);
		Config &operator = (const Config &config);
		~Config();




	private:
		std::vector<ServerConfig>	servers;
};

#endif