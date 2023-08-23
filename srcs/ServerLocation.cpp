#include "webserv.hpp"

namespace conf
{
	ServerLocation::ServerLocation(std::ifstream *file)
	{
		string text, var1, var2, key, value;
		std::vector<string> tmp, value_vec;

		while (std::getline(*file, text))
		{
			if (text[1] == '}')
			{
				// std::map<string, std::vector<string> >::iterator it;
				// for (it = rules.begin(); it != rules.end(); it++)
				// {
				// 	cout << "key : " << it->first << endl;
				// 	cout << "value: " << endl;
				// 	for (size_t i = 0; i < it->second.size(); ++i)
				// 		cout << "	" << it->second[i] << endl;
				// 	cout << endl;
				// };
				// cout << endl;
				break;
			}
			if (text.find('#') == std::string::npos)
			{
				std::stringstream ss(text);
				if (text.empty())
						continue;
				if (text.find("return") == std::string::npos)
				{
					while (std::getline(ss, key, '	'))
					{
						if (key.empty())
							continue;
						else
							tmp.push_back(key);
					}
				}
				else
				{
					std::istringstream iss(text);
					iss >> var1 >> var2;
					tmp.push_back(var1);
					tmp.push_back(var2);
				}
				std::stringstream v(tmp[1]);
				while (std::getline(v, value, ' '))
					value_vec.push_back(value);
				this->rules[tmp[0]] = value_vec;
				value_vec.clear();
				tmp.clear();
			}
		}
	}

	ServerLocation::ServerLocation(const ServerLocation &L)
	{
		*this = L;
	}

	ServerLocation	&ServerLocation::operator=(const ServerLocation &L)
	{
		if (this != &L)
		{

		}
		return (*this);
	}

	ServerLocation::~ServerLocation()
	{}
}