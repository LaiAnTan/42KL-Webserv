#include "WebSurf.hpp"

namespace HDE
{
	int	WebSurf::timeout = 10 * 60 * 1000;

	WebSurf::WebSurf(const conf::Config *config)
	{
		this->ng_config = config;
		this->server_count = 0;
		this->doorbell_count = 0;

		init_doorbells();
	}

	WebSurf::~WebSurf()
	{
		// do we have garbage collectors here? idts right? FUCK
	}

	// i dont know why i called this doorbells
	// oh well
	void	WebSurf::init_doorbells()
	{
		const std::vector<conf::ServerConfig> &config_server = ng_config->get_servers();

		std::vector<conf::ServerConfig>::const_iterator	x = config_server.begin();
		for (;x != config_server.end(); ++x)
		{
			add_doorbell(&(*x));
		}

		cout << YELLOW << "[INFO] Added " << doorbell_count << " doorbells!" << endl;
	}

	void	WebSurf::run_servers()
	{
		// amen
		// barebone poll inplementation
		// will implement errors later
		int	ret_value;
		int	total_fds;
		int	run_server = true;
		Server	*current;

		while (run_server)
		{
			total_fds = doorbell_count + server_count;


			cout << CYAN << "[PROMPT] --------------------------" << endl;
			cout << "Number of Connection Points    || " << doorbell_count << endl;
			cout << "Number of Serving Servers      || " << server_count << endl;
			cout << "Total Number of Sockets Active || " << total_fds << endl;
			cout << "-------------------------------------" << endl;
			cout << "PORTS IN USE" << endl;
			cout << "-------------------------------------" << endl;
			for (doorbell_iter start = doorbells.begin(); start != doorbells.end(); ++start)
				cout << "PORT " << start->second->get_port() << endl;
			cout << endl;
			cout << CYAN << "[PROMPT-END] ----------------------" << endl;
			cout << RESET;

			ret_value = poll(&pfds[0], total_fds, WebSurf::timeout);
			if (ret_value < 0)
			{
				cerr << RED << "[ERROR] Something Went wrong with poll" << endl;
				break;
			}
			if (ret_value == 0)
			{
				cout << YELLOW << "[INFO] Poll time-out, Exiting..." << endl;
				break;
			}
			else
			{
				for (int x = 0; x < total_fds; ++x)
				{
					if (pfds[x].revents == 0)
						continue;

					// new connection to server
					if (x < doorbell_count)
					{
						if (pfds[x].revents != POLLIN)
						{
							cerr << RED << "[ERROR] Error at Doorbell fd " << pfds[x].fd << endl;
							run_server = false;
							break;
						}
						else
						{
							ServerListener	*ringing = doorbells[pfds[x].fd];
							add_server(ringing);
						}
					}
					// connected clients
					else
					{
						if (pfds[x].revents & POLLIN)
						{
							cout << BLUE << "[NOTICE] Socket at " << pfds[x].fd << " is receiving data" << endl;
							cout << RESET;
							current = servers[pfds[x].fd];
							// if read returns zero, socket has disconnected, remove the server
							if (!current->accepter())
								remove_server(pfds[x].fd);
							else
								pfds[x].events = POLLOUT;
						}
						else if (pfds[x].revents & POLLOUT)
						{
							cout << BLUE << "[NOTICE] Socket at " << pfds[x].fd << " is sending data" << endl;
							cout << RESET;

							cout << CYAN << "[HANDLER] --------------------------" << endl;
							cout << "WHAT WAS READ" << RESET << endl;
							current->handler();
							cout << CYAN << "[HANDLER-END] ----------------------" << endl;

							cout << GREEN << "[RESPONDER] -----------------------" << endl;
							cout << RESET;
							current->responder();
							cout << GREEN << "[RESPONDER-END] -------------------" << endl;

							// no keep-alive?
							// ------------------------------------------
							// ⠀⣞⢽⢪⢣⢣⢣⢫⡺⡵⣝⡮⣗⢷⢽⢽⢽⣮⡷⡽⣜⣜⢮⢺⣜⢷⢽⢝⡽⣝
							// ⠸⡸⠜⠕⠕⠁⢁⢇⢏⢽⢺⣪⡳⡝⣎⣏⢯⢞⡿⣟⣷⣳⢯⡷⣽⢽⢯⣳⣫⠇
							// ⠀⠀⢀⢀⢄⢬⢪⡪⡎⣆⡈⠚⠜⠕⠇⠗⠝⢕⢯⢫⣞⣯⣿⣻⡽⣏⢗⣗⠏⠀
							// ⠀⠪⡪⡪⣪⢪⢺⢸⢢⢓⢆⢤⢀⠀⠀⠀⠀⠈⢊⢞⡾⣿⡯⣏⢮⠷⠁⠀⠀
							// ⠀⠀⠀⠈⠊⠆⡃⠕⢕⢇⢇⢇⢇⢇⢏⢎⢎⢆⢄⠀⢑⣽⣿⢝⠲⠉⠀⠀⠀⠀
							// ⠀⠀⠀⠀⠀⡿⠂⠠⠀⡇⢇⠕⢈⣀⠀⠁⠡⠣⡣⡫⣂⣿⠯⢪⠰⠂⠀⠀⠀⠀
							// ⠀⠀⠀⠀⡦⡙⡂⢀⢤⢣⠣⡈⣾⡃⠠⠄⠀⡄⢱⣌⣶⢏⢊⠂⠀⠀⠀⠀⠀⠀
							// ⠀⠀⠀⠀⢝⡲⣜⡮⡏⢎⢌⢂⠙⠢⠐⢀⢘⢵⣽⣿⡿⠁⠁⠀⠀⠀⠀⠀⠀⠀
							// ⠀⠀⠀⠀⠨⣺⡺⡕⡕⡱⡑⡆⡕⡅⡕⡜⡼⢽⡻⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
							// ⠀⠀⠀⠀⣼⣳⣫⣾⣵⣗⡵⡱⡡⢣⢑⢕⢜⢕⡝⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
							// ⠀⠀⠀⣴⣿⣾⣿⣿⣿⡿⡽⡑⢌⠪⡢⡣⣣⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
							// ⠀⠀⠀⡟⡾⣿⢿⢿⢵⣽⣾⣼⣘⢸⢸⣞⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
							// ⠀⠀⠀⠀⠁⠇⠡⠩⡫⢿⣝⡻⡮⣒⢽⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
							// ------------------------------------------

							remove_server(pfds[x].fd);

							// pfds[x].events = POLLIN;
						}
						else
						{
							cerr << RED << "[ERROR] Error at Server fd " << pfds[x].fd << endl;
							if (pfds[x].revents & POLLHUP)
								cout << YELLOW << "[INFO] Client Has Hung Up on server" << endl;
							remove_server(pfds[x].fd);
						}
					}
				}
			}
		}
	}

	bool	WebSurf::remove_server(int server_fd)
	{
		// close fd connection
		cout << BLUE << "[NOTICE] Closing connection to file descriptor " << server_fd << endl;
		close(server_fd);

		// search for iterator 
		pollfd_iter current = pfds.begin();
		for (; current != pfds.end(); ++current)
		{
			if ((*current).fd == server_fd)
				break;
		}

		// remove from polling fd list
		pfds.erase(current);

		// remove from map
		this->servers.erase(server_fd);

		--this->server_count;
		return true;
	}

	bool	WebSurf::add_server(ServerListener *doorbell)
	{
		Server *client = doorbell->accept_connection();
		int	fd = client->get_socket();
		pollfd	client_poll;

		// add polling fd
		client_poll.events = POLLIN;
		client_poll.fd = fd;
		pfds.push_back(client_poll);

		// add entry into map
		servers.insert(server_node(fd, client));

		cout << BLUE << "[NOTICE] Added socket fd " << fd << endl;
		++this->server_count;
		return true;
	}

	bool	WebSurf::add_doorbell(const	conf::ServerConfig *config)
	{
		std::vector<std::string>			port_list = config->get_port();
		std::vector<std::string>::iterator	port_iter = port_list.begin();
		ServerListener						*new_doorbell;
		int									fd;

		for (; port_iter != port_list.end(); ++port_iter)
		{
			new_doorbell = new ServerListener(config, (*port_iter));
			fd = new_doorbell->get_socket();

			// i am so betting on this
			pollfd	doorbell_poll;

			doorbell_poll.events = POLLIN;
			doorbell_poll.fd = fd;
			pfds.push_back(doorbell_poll);

			// add entry to map
			doorbells.insert(doorbell_node(fd, new_doorbell));

			++this->doorbell_count;
		}
		return true;
	}
}