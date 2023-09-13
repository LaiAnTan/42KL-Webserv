#include "Server.hpp"

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

void dataGet(int socket)
{
	string headers = HDE::Server::get_headers();
	string content = HDE::Server::get_content();
	string boundary, filename, path;

	if (headers.find("POST") != string::npos)
	{
		path = headers.substr(headers.find("POST ") + 5);
		path = path.substr(0, path.find(" "));
		if (path.find("/login") != string::npos)
			login(content, socket);
		else
		{
			size_t boundaryPos = headers.find("boundary=");
			boundary = headers.substr(boundaryPos + 9);
			boundary = boundary.substr(0, boundary.find("\r"));

			size_t filenamePos = content.find("filename=");
			filename = content.substr(filenamePos + 10);
			filename = filename.substr(0, filename.find("\""));

			cout << "Boundary: |" << boundary << "|" << endl;
			// cout << "Filename: |" << filename << "|" << endl;

			size_t dataPos = content.find("\r\n\r\n") + 4;
			size_t boundaryPosInData = content.find("--" + boundary, dataPos);
			std::string fileData = content.substr(dataPos, boundaryPosInData - dataPos);
			filename = "./upload/" + filename;
			cout << "File Data: |" << filename << "|" << endl;
			std::ofstream outFile(filename.c_str(), std::ios::binary);
			outFile.write(fileData.c_str(), fileData.length());
			outFile.close();
		}
	}
}