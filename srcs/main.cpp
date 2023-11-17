#include "WebSurf.hpp"

int main(int ac, char **av)
{
	if (ac == 2)
	{
		string file = av[1];
		try
		{
			conf::Config	configuration(file);
			cout << configuration << endl;
			HDE::WebSurf		server(&configuration);
			server.run_servers();
			// HDE::Server		server(&configuration);

		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << endl;
		}
	}
	else
		cout << RED "Error: ./webserv [configuration file]" RESET << endl;
}