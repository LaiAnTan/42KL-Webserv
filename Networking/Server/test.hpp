#ifndef TEST_HPP
#define TEST_HPP

#include "SimpleServer.hpp"
#include <vector>

namespace HDE
{
	class test : public SimpleServer
	{
	private:
		char buffer[30000];
		int newsocket;

		void accepter();
		void handler();
		void responder();
		static std::vector<string> bufferVEC;

	public:
		test(/* args */);
		// ~test();
		void launch();
		static std::vector<string> get_bufferVEC();
	};
}

int sendData(int sckt, const void *data, int datalen);
void dataSet(int socket);

#endif