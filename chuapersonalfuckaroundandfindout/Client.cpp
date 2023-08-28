#include "Client.hpp"

namespace HDE
{
	// can change
	int	Client::buffer_size = 1000000;

	Client::Client()
	{}

	Client::Client(Client &other)
	{
		*(this) = other;
	}

	Client::Client(int prov_sock) : client_socket(prov_sock)
	{
	}

	Client::~Client()
	{}

	Client	&Client::operator=(Client &other)
	{
		if (&other != this)
		{
			this->client_socket = other.client_socket;
			this->content = other.content;
		}
		return (*(this));
	}

	// reads data from the socket
	// expect that socket has data to read

	// update, read doesnt work for some reason
	// once fd close it just epicly infinitely loop like theres no tomorrow
	// w e l p
	int	Client::c_read()
	{
		int		len;
		int		ret;
		char	buff[Client::buffer_size + 1];

		// clean buffer
		memset(buff, 0, (Client::buffer_size + 1));

		len = recv(this->client_socket, buff, Client::buffer_size, 0);
		cout << GREEN << "Received " << len << " data" << endl;
		// client closed connection :P
		if (len == 0)
			return -1;

		// temp [DATA IS DONE READING] holder
		ret = (buff[len - 1] != '\0');
		if (ret)
		{
			cout << MAGENTA << "Data is halfway recieved" << endl;
			cout << MAGENTA << "Last character was " << buff[len - 1] << " (" << int(buff[len - 1]) << ")" << endl;
		}
		else
			cout << MAGENTA << "Data is fully received" << endl;
		// will DEFO CHANGE depending on how the request message is sent

		buff[len] = '\0';
		cout << GREEN << "Recieved Messsage " << buff << endl;
		this->content.append(buff);
		return (ret);
	}

	// int	Client::c_read()
	// {
	// 	int		len;
	// 	int		ret;
	// 	char	buff[Client::buffer_size + 1];

	// 	len = read(this->client_socket, buff, Client::buffer_size);
	// 	// temp [DATA IS DONE READING] holder
	// 	ret = (buff[len] != '\0');
	// 	buff[len] = '\0';
	// 	// will DEFO CHANGE depending on how the request message is sent
	// 	this->content.append(buff);

	// 	// idt i can use this
	// 	// ioctl(this->client_socket, FIONREAD, &len);
	// 	// if (len > 0) {
	// 	// }
	// 	return (ret);
	// }

	// placeholder for the real processing info
	int	Client::c_process()
	{
		cout << RESET << endl;
		cout << "----------------------------------------------------" << endl;
		cout << BLUE << "In Client = " << client_socket << endl;
		cout << RESET << content << endl;
		cout << "----------------------------------------------------" << endl;
		cout << endl;

		content.clear();
		return (1);
	}

	int	Client::c_write(char *to_write)
	{
		// i handle chunks later :P
		int	ret;
		int	length = -1;

		while (to_write[++length]){};

		ret = send(this->client_socket, to_write, length + 1, 0);
		return (ret);
	}

	int	Client::c_write()
	{
		int	ret;

		// that plus one is to send the '\0' also

		// for testing purposes ONLY
		ret = send(this->client_socket, to_send.c_str(), (to_send.length() + 1), 0);
	}

	int	Client::c_write(string &to_write)
	{
		// i handle chunks later :P
		int	ret;
	
		ret = send(this->client_socket, to_write.c_str(), (to_write.length() + 1), 0);
		return (ret);
	}

	int	Client::get_Socket()
	{
		return (client_socket);
	}

	string	&Client::get_Content()
	{
		return (content);
	}
}