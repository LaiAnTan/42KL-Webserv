// main poll looper

#ifndef WebSurf_HPP
#define WebSurf_HPP

#include <poll.h>

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>

#include "server/ServerListener.hpp"
#include "server/Server.hpp"
#include "config/Config.hpp"

namespace HDE
{
	using std::cerr;

	class WebSurf
	{
		private:
			const conf::Config	*ng_config;

			typedef	struct pollfd					pollfd_struct;
			typedef	std::vector<pollfd_struct>		pollfd_list;
			typedef	pollfd_list::iterator			pollfd_iter;

			typedef std::map<int, Server *>			server_list;
			typedef	server_list::iterator			server_iter;
			typedef	std::pair<int, Server *>		server_node;

			typedef std::map<int, ServerListener *>		doorbell_list;
			typedef	doorbell_list::iterator				doorbell_iter;
			typedef	std::pair<int, ServerListener *>	doorbell_node;

			static int		timeout;

			pollfd_list		pfds;

			doorbell_list	doorbells;
			int				doorbell_count;

			server_list		servers; 
			int				server_count;

			int				total_count;

			void	init_doorbells();

			bool	add_doorbell(const conf::ServerConfig *config);
			bool	remove_server(int server_fd, int *counter);
			bool	add_server(ServerListener *doorbell);

		public:
			WebSurf(const conf::Config *config);
			~WebSurf();

			void	run_servers();
			void	print_status();
	};
} // namespace HDE

#endif