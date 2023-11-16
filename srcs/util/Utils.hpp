#pragma once

#ifndef UTILS_HPP

#define UTILS_HPP

#include <vector>
#include <string>
#include <sstream>

namespace util
{
	std::vector<std::string>	split(std::string str, std::string delim);
	std::vector<std::string>	split_many_delims(std::string str, std::string delims);
} // namespace util




#endif