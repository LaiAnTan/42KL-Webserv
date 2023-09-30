#include "Server.hpp"

using std::cout;
using std::endl;
using std::cerr;

namespace HDE
{

	void	Server::handleDeleteRequest()
	{

	}

	void	Server::createDeleteResponse(int socket, string content, string content_type, bool is_deleted)
	{
		string	response; // response string to be sent to client

		/*
		codes:
		- 202 Accepted- accepted but havent delete
		- 200 OK - accepted and deleted, response contains content
		- 204 No Content - accepted and deleted, response dont have content
		(stay on same page)
		*/ 

		if (is_deleted == false)
			response.append("HTTP/1.1 202 Accepted\r\n");
		else
		{
			if (content.empty() == true)
				response.append("HTTP/1.1 204 No Content\r\n");
			else
			{
				response.append("HTTP/1.1 200 OK\r\n");
				
				// append content type & content here
			}
		}

		if (sendData(socket, (void *) response.c_str(), response.size()) != 0)
			cerr << "Error sending response" << endl;
		return ;
	}
}