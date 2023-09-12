#ifndef TEST_HPP
#define TEST_HPP

#include "SimpleServer.hpp"
#include <vector>

namespace HDE
{
	class test : public SimpleServer
	{
	private:
		int newsocket;
		static string headers;
		static string content;

		void accepter();
		void handler();
		void responder();

	public:
		test(/* args */);
		// ~test();
		void launch();
		// static std::vector<string> get_bufferVEC();
		// static char	*get_bufferCHAR();
		static string get_headers();
		static string get_content();
	};
}

int sendData(int sckt, const void *data, int datalen);
void dataSet(int socket);
void dataGet(int socket);

#endif