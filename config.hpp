#pragma once

#ifndef CONFIG_HPP

#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

using std::string;

class ServerLocation
{
	public:

	private:
		std::map<string, std::vector<string>>	rules;
};

class ServerConfig
{
	public:
		ServerConfig();
		ServerConfig(const ServerConfig &server_config);
		ServerConfig &operator = (const ServerConfig &server_config);
		~ServerConfig();


	private:
		std::vector<string>					listen;
		// stuff


		std::map<string, ServerLocation>	locations;

};

class Config
{
	public:
		Config();
		// Construtor to handle the whole config file
		Config(std::string filename);
		Config(const Config &config);
		Config &operator = (const Config &config);
		~Config();

	private:
		std::vector<ServerConfig>	servers;
};

#endif