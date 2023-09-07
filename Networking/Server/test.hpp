#ifndef TEST_HPP
#define TEST_HPP

#include "SimpleServer.hpp"

namespace HDE
{
	class test : public SimpleServer
	{
	private:
		char buffer[30000] = {0};
		int newsocket;

		void accepter();
		void handler();
		void responder();
	public:
		test(/* args */);
		// ~test();
		void launch();
	};
}

int sendData(int sckt, void *data, int datalen);
void dataSet(int socket, void *data);

#endif