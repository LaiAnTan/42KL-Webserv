#include "webserv.hpp"

int main(int ac, char **av)
{
	if (ac == 2)
	{
		string file = av[1];
		try
		{
			conf::Config	configuration(file);
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << endl;
		}
	}
	else
		cout << RED "Error: ./webserv [configuration file]" RESET << endl;
}