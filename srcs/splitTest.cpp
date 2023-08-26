#include "Utils.hpp"

#include <iostream>

int main(void)
{
	std::string	s;
	std::string s2;

	s = "Hello World! Foo Bar Baz";
	s2 = "testnodelimiter";

	std::vector<std::string> v;

	v = util::split(s2, " ");

	std::vector<std::string>::iterator	start = v.begin();
	std::vector<std::string>::iterator	end = v.end();

	for (start; start != end; start++)
		std::cout << *start << std::endl;

	return (0);
}