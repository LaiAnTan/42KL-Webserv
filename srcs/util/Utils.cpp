#include "Utils.hpp"

namespace util
{
	std::vector<std::string>	split(std::string str, std::string delim)
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

	std::vector<std::string>	split_many_delims(std::string str, std::string delims)
	{
		std::string					line;
		std::stringstream			ss(str);
		std::vector<std::string>	tokens;

		while(std::getline(ss, line)) 
		{
			std::size_t prev = 0, pos;
			while ((pos = line.find_first_of(delims, prev)) != std::string::npos)
			{
				if (pos > prev)
					tokens.push_back(line.substr(prev, pos - prev));
				prev = pos + 1;
			}
			if (prev < line.length())
				tokens.push_back(line.substr(prev, std::string::npos));
		}
		return (tokens);
	}
}
