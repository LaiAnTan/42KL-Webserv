#include "Server.hpp"
#include "../config/Config.hpp"
#include <sys/stat.h>

using std::cout;
using std::endl;

namespace HDE
{
	// extracts login details from header, returns a pair
	std::pair<string, string>	extract_login_details(string header)
	{
		size_t un_loc;
		size_t pw_loc;
		std::pair<string, string>	details;

		un_loc = header.find("username=");
		pw_loc = header.find("&password=");

		if (un_loc == string::npos || pw_loc == string::npos) // if errror return empty pair
		{
			details.first = "";
			details.second = "";
		}
		else
		{
			details.first = header.substr(un_loc + 9, pw_loc - (un_loc + 9));
			details.second = header.substr(pw_loc + 10);
		}
		return (details);
	}

	void Server::handlePostRequest(int socket)
	{
		string	root; // root directory
		string	headers = get_headers();
		string	content = get_content();
		string	boundary, filename, path;
		size_t	boundaryPos = headers.find("boundary=");
		string	respones_filename = "./html/200.html";

		root = "root";
		path = headers.substr(headers.find("POST ") + 5);
		path = path.substr(0, path.find(" "));
		boundary = headers.substr(boundaryPos + 9);
		boundary = boundary.substr(0, boundary.find("\r"));

		size_t nextBoundaryPos = content.find(boundary) + 1;
		string nextContent = content.substr(nextBoundaryPos);
		string endBoundary = boundary + "--";

		while (true)
		{
			if (strncmp(nextContent.c_str(), ("-" + endBoundary).c_str(), endBoundary.size()) == 0)
				break;
			if (nextContent.find("filename=") != string::npos)
			{
				// extract filename
				size_t filenamePos = nextContent.find("filename=");
				filename = nextContent.substr(filenamePos + 10);
				filename = filename.substr(0, filename.find("\""));
				cout << "Filename: " << "|" + filename + "|" << endl;

				// extract content
				cout << RED << nextContent << RESET << endl;
				size_t dataPos = nextContent.find("\r\n\r\n") + 4;
				size_t boundaryPosInData = nextContent.find("--" + boundary, dataPos);

				// check if boundaryPosInData is npos
				if (boundaryPosInData == string::npos)
				{
					cout << RED << "boundaryPosInData is npos" << RESET << endl;
					respones_filename = "./html/400.html";
					break;
				}
				string fileContent = nextContent.substr(dataPos, boundaryPosInData - dataPos);

				// write into file
				string	path = "./" + root + "/" + filename;
				std::ofstream outFile(path.c_str(), std::ios::binary);
				outFile.write(fileContent.c_str(), fileContent.length());
				outFile.close();

			}
			size_t nextBoundaryPos = nextContent.find("--" + boundary) + 1;
			nextContent = nextContent.substr(nextBoundaryPos);
		}
		handlePostResponse(respones_filename, socket);
		return ;
	}

	void	Server::handlePostResponse(string filename, int socket)
	{
		Server::handleGetResponse(filename, socket);
	}
}