// alright boys start praying to whatever god you believe in that this works
// fuck me :P

# include "WebServer.hpp"

int main()
{
	WebServer	runner;

	// loopback
	// runner.ConnectServer(6969, "127.0.0.1");
	runner.ConnectServer(6969);
	// run thy server
	cout << endl;
	cout << GREEN << "Starting Server..." << endl;
	runner.RunServer();
	cout << endl;
	cout << BLUE << "Server Closed" << endl;
}