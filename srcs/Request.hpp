#pragma once

#ifndef REQUEST_HPP

#define REQUEST_HPP

#include <map>
#include <vector>
#include <string>
#include <ostream>
#include <fstream>
#include <iostream>
#include <exception>

namespace req
{
	/* 
	Class that parses and handles client requests
	For request format, refer to RFC section 5
	*/
	class Request
	{
		public:
			typedef std::map<std::string, std::vector<std::string> >	requestHeader;

			Request();
			Request(std::ifstream *file);
			Request(const Request &request);
			Request &operator = (const Request &request);
			~Request();
			
			// getters
			std::string			getMethod(void) const;
			std::string			getTarget(void) const;
			std::string			getVersion(void) const;
			const requestHeader	&getRequestHeader(void) const;

			// setters
			void	setMethod(std::string method);
			void	setTarget(std::string target);
			void	setVersion(std::string version);

			// request header field methods
			// adds / updates header with a vector of values
			void	updateHeaderField(std::string field_name, std::vector<std::string> field_values);
			// adds / updates header with a value
			void	updateHeaderField(std::string field_name, std::string field_value);

			// more methods to come
			
			// exceptions
			class WrongRequestFormatException: public std::exception
			{
				public:
					const char* what() const throw();
			};

		private:
			// request method
			std::string		method;
			// request target
			std::string		target;
			// http version 
			std::string		ver;
			// request header
			requestHeader	header;

			// sockets
			int				server_fd;
			int				client_fd;
	};

	std::ostream	&operator << (std::ostream outs, const Request &request);

} // namespace req

#endif