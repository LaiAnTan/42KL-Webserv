// alright boys start praying to whatever god you believe in that this works
// fuck me :P

# include "WebServer.hpp"

int main()
{
	WebServer	runner;

	// loopback
	// runner.ConnectServer(6969, "127.0.0.1");

	// ip address should be connect to here
	// do ipconfig, then grab the wifi's ip

	runner.ConnectServer(6969);

	// run thy server
	cout << endl;
	cout << GREEN << "Starting Server..." << endl;
	runner.RunServer();
	cout << endl;
	cout << BLUE << "Server Closed" << endl;
}