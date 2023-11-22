#include "WebSurf.hpp"

int main(int ac, char **av)
{
	// stop pipe from dying!!!
	signal(SIGPIPE, SIG_IGN);
	if (ac == 2)
	{
		string file = av[1];
		try
		{
			conf::Config	configuration(file);
			cout << configuration << endl;
			HDE::WebSurf		server(&configuration);
			server.run_servers();
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << endl;
		}
	}
	else
		cout << RED "Error: ./webserv [configuration file]" RESET << endl;
}

