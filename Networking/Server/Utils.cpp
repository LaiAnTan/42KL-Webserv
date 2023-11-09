#include "Utils.hpp"

namespace util
{
	std::vector<std::string>	split(std::string &str, std::string	delim)
	{
		size_t 						pos = str.find(delim);
		std::vector<std::string>	tokens;
		
		while (pos != std::string::npos)
		{
			tokens.push_back(str.substr(0, pos));
			str.erase(0, pos + delim.length());
			pos = str.find(delim);
		}
		tokens.push_back(str.substr(0, str.length()));
		return (tokens);
	}
}

