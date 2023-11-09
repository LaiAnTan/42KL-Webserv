#include "Server.hpp"
#include <sys/stat.h>

namespace HDE
{
	// extracts login details from header, returns a pair
	std::pair<string, string>	extract_login_details(string header, int socket)
	{
		(void) socket;
		size_t un_loc;
		size_t pw_loc;
		std::pair<string, string>	details;

		un_loc = header.find("username=");
		pw_loc = header.find("&password=");

		if (un_loc == string::npos || pw_loc == string::npos) // if errror return empty pair
			details.first, details.second = "";
		else
		{
			details.first = header.substr(un_loc + 9, pw_loc - (un_loc + 9));
			details.second = header.substr(pw_loc + 10);
		}
		return (details);
	}

	void login(string content, int sock)
	{
		(void)sock;
		string username = "demo";
		string password = "password";

		size_t usernamePos = content.find("username=");
		size_t passwordPos = content.find("&password=");

		if (usernamePos != string::npos && passwordPos != string::npos)
		{
			// Extract username and password from the content
			string extractedUsername = content.substr(usernamePos + 9, passwordPos - (usernamePos + 9));
			string extractedPassword = content.substr(passwordPos + 10);

			cout << "Username: " << extractedUsername << endl;
			cout << "Password: " << extractedPassword << endl;

			// Compare extracted values with hardcoded values
			if (extractedUsername == username && extractedPassword == password)
			{
				// response.append("<h1>Login Successful</h1>");
				cout << "Login Successful" << endl;
			}
			else
			{
				// response.append("<h1>Login Failed</h1>");
				cout << "Login Failed" << endl;
			}
		}
		// int res = sendData(sock, (void *)response.c_str(), response.size());
	}

	// depreciated function
	int	create_user_dir(string root, string username)
	{
		int			child_pid;
		string		path;
		struct stat	info;

		path = "./" + root + "/" + username;

		cout << path << endl;
		if (stat(path.c_str(), &info) != 0)
		{
			mkdir(path.c_str(), 0755); // change this later
			if (open(path.c_str(), O_CREAT | O_DIRECTORY, 0755) < 0)
				return (-1);
		}
		return (0);
	}

	void Server::dataGet(int socket)
	{
		string	headers = get_headers();
		string	content = get_content();
		string	boundary, filename, path;
		string	root; // root directory

		root = "root";

		if (headers.find("POST") != string::npos)
		{

			path = headers.substr(headers.find("POST ") + 5);
			path = path.substr(0, path.find(" "));

			size_t boundaryPos = headers.find("boundary=");
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
					size_t dataPos = nextContent.find("\r\n\r\n") + 4;
					size_t boundaryPosInData = nextContent.find("--" + boundary, dataPos);
					string fileContent = nextContent.substr(dataPos, boundaryPosInData - dataPos);

					// write into file
					filename = "./" + root + "/" + filename;
					std::ofstream outFile(filename.c_str(), std::ios::binary);
					outFile.write(fileContent.c_str(), fileContent.length());
					outFile.close();

				}
				size_t nextBoundaryPos = nextContent.find("--" + boundary) + 1;
				nextContent = nextContent.substr(nextBoundaryPos);
			}
		}
	}
}