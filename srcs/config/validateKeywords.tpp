#include "Config.hpp"
#include <map>
#include <set>
#include <iterator>

namespace conf
{
	template <typename T, typename U>
	bool	validateKeywords(std::set<std::string> keywords, std::map<T, U> &map)
	{
		typename std::map<T, U>::const_iterator	it = map.begin();

		for (; it != map.end(); ++it)
		{
			if (keywords.find(it->first) == keywords.end())
				return (false);
		}
		return (true);
	}
}