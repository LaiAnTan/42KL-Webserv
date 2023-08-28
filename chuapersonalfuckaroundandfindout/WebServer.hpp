#ifndef __WEBSERVER_HPP__
#define __WEBSERVER_HPP__

#include <vector>
#include <map>
#include "Server.hpp"
#include "Client.hpp"
#include <poll.h>

#include <errno.h>
#include <string.h>

// can we use algorithm?
#include <algorithm>

using std::vector;
using std::map;
using HDE::Server;
using HDE::Client;
using std::pair;
using std::find;
using std::remove;
using std::for_each;

class WebServer
{
	private:
		// ALOT of angry error when i tried to store the object
		// sooo we storing points now :(
		typedef	struct pollfd			fd_container;
		typedef	vector<fd_container>	fd_list;
		typedef fd_list::iterator		fd_listiter;

		typedef	map<int, Server*>			server_list;
		typedef	server_list::iterator		server_iter;
		typedef pair<const int, Server*>	server_node;

		typedef map<int, Client*>			client_list;
		typedef client_list::iterator		client_iter;
		typedef pair<const int, Client*>	client_node;

		fd_list		fds;
		server_list	servers;
		client_list	clients;

		int			client_num;
		int			server_num;

		static void				for_each_server(server_node	&node);
		static void				for_each_client(client_node	&node);
		WebServer::fd_listiter	Find_Fd(fd_container tofind);

		static int	timeout;
	public:
		WebServer();

		void	RunServer();

		void					ConnectServer(int port);
		void					ConnectServer(int port, char *ip);
		void					ConnectClient(Server &server);
		void					RemoveClient(fd_container client_fd);

		~WebServer();
};



#endif