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

using std::endl;
using std::cout;
using std::string;

namespace resp
{
	class Respond
	{
		public:
			Respond();
			~Respond();

			//getter
			string	getRequestURL();
			string	getRequestMethod();
			string	getCode();
			string	getContentType();

			//setter 
			void	setRequestURL(string var);
			void	setRequestMethod(string var);
			void	setCode(string var);
			void	setContentType(string var);

		private:
			//request URL
			string	req_url;
			//request Method
			string	req_method;
			//status code
			string	code;
			//content type
			string	content_type;


	};
} // namespace req

#endif