#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <fstream>
#include <exception>
#include "../server/Server.hpp"
#include "validateKeywords.tpp"
#include "../util/Utils.hpp"

# define RESET		"\033[0m"
# define BLACK		"\033[1;30m"
# define RED		"\033[1;31m"
# define GREEN		"\033[1;32m"
# define YELLOW		"\033[1;33m"
# define BLUE		"\033[1;34m"
# define MAGENTA	"\033[1;35m"
# define CYAN		"\033[1;36m"
# define WHITE		"\033[1;37m"

using std::string;
using std::cout;
using std::endl;

namespace conf
{
	class ServerLocation
	{
		public:

			typedef std::map<string, std::vector<string> > rules_map;

			ServerLocation(std::ifstream *file);
			ServerLocation(const ServerLocation &L);
			ServerLocation &operator = (const ServerLocation &L);
			ServerLocation();
			~ServerLocation();

			//setter
			void	set_root(string text);
			void	set_index(string text);
			void	set_autoindex(string text);
			void	set_client_max_body_size(string text);
			void	set_return_path(string text);
			void 	set_allowed_method(string text);

			//getter
			string	get_root() const;
			string	get_index() const;
			string	get_autoindex() const;
			string	get_client_max_body_size() const;
			string	get_return_path() const;
			const std::vector<string>	&get_allowed_method() const;


			const std::map<string, std::vector<string> > &get_rules() const;

		private:
			string					root;
			string					index;
			string					autoindex;
			string					client_max_body_size;
			string 					return_path;
			std::vector<string>		allowed_method;

			std::set<string>						valid_keywords;
			std::map<string, std::vector<string> >	rules;
	};

	std::ostream &operator << (std::ostream &outs, const ServerLocation &server_location);

	class ServerConfig
	{
		public:
			typedef	std::vector<string>::iterator		portIter;
			typedef std::map<string, ServerLocation>	locationMap;

			ServerConfig(std::ifstream *file);
			ServerConfig(const ServerConfig &server_config);
			ServerConfig &operator = (const ServerConfig &server_config);
			~ServerConfig();

			//setter
			void	set_port(string text);
			void	set_root(string text);
			void	set_index(string text);
			void	set_server_name(string text);
			void	set_client_max(string text);
			void	set_error(string text);
			void	set_cgi(string text);
			void	set_methods(string text);
			void	location_name(string text, std::ifstream *file);

			//getter
			string	get_root() const;
			string	get_index() const;
			string	get_server_name() const;
			string	get_client_max() const;
			const std::vector<string>		&get_port() const;
			const std::map<string, string>	&get_error() const;
			const std::map<string, string>	&get_cgi() const;
			const std::vector<string>		&get_methods() const;
			const std::map<string, ServerLocation>	&get_locations() const;

		private:
			std::vector<string>					port;
			string								root;
			string								index;
			string								server_name;
			string								client_max;

			std::map<string, string>			error;
			std::map<string, string>			cgi;
			std::vector<string>					allowed_method;
			// stuff

			std::set<string>					valid_keywords;
			std::map<string, ServerLocation>	locations;

	};

	std::ostream &operator << (std::ostream &outs, const ServerConfig &server_config);

	class Config
	{
		public:
			Config();
			// Construtor to handle the whole config file
			Config(std::string filename);
			Config(const Config &config);
			Config &operator=(const Config &config);
			~Config();
			void	config_handle(std::ifstream *file);

			//getter
			const std::vector<ServerConfig> &get_servers() const;
		private:
			std::vector<ServerConfig>	servers;
	};

	std::ostream &operator << (std::ostream &outs, const Config &config);

	class Error : public std::exception
	{
		public:
			Error(const char* msg) : message(msg) {}

			virtual const char* what() const throw()
			{
				return message;
			}
		private:
			const char* message;
	};

	class InvalidKeywordException: public std::exception
	{
		public:
			const char* what() const throw();
	};

	class MissingSemicolonException: public std::exception
	{
		public:
			const char* what() const throw();
	};

	class InvalidSuffixException: public std::exception
	{
		public:
			const char* what() const throw();
	};

	class InvalidValueException: public std::exception
	{
		public:
			const char* what() const throw();
	};

	class TooManyValuesException: public std::exception
	{
		public:
			const char* what() const throw();
	};

	template <typename T, typename U> bool	validateKeywords(std::set<std::string> keywords, std::map<T, U>	&map);
}

#endif

