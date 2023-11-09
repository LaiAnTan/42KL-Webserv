#include "ListeningSocket.hpp"

int main()
{
	std::cout << "Starting....." << std::endl;
	// std::cout << "Simple Socket....." << std::endl;
	// HDE::SimpleSocket ss = HDE::SimpleSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);
	std::cout << "Binding Socket....." << std::endl;
	HDE::BindingSocket bs = HDE::BindingSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);
	std::cout << "Listening Socket....." << std::endl;
	HDE::ListeningSocket ls = HDE::ListeningSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10);

}


//g++ main.cpp SimpleSocket.cpp ListeningSocket.cpp BindingSocket.cpp -o test