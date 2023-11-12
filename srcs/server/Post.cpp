#include "Server.hpp"
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

	void	writeIntoFile(string file_content, string file_path)
	{
		std::ofstream	output_file(file_path.c_str(), std::ios::binary);

		output_file.write(file_content.c_str(), file_content.length());
		output_file.close();
	}

	// int Server::handlePostRequest()
	// {
	// 	string	root; // root directory
	// 	string	headers = get_headers();
	// 	string	content = get_content();
	// 	string	boundary, filename, path;
	// 	size_t boundaryPos = headers.find("boundary=");

	// 	// header stuff
	// 	root = "root";
	// 	path = headers.substr(headers.find("POST ") + 5);
	// 	path = path.substr(0, path.find(" "));
	// 	boundary = headers.substr(boundaryPos + 9);
	// 	boundary = boundary.substr(0, boundary.find("\r"));
		
	// 	// funny thing here that i need to change
	// 	size_t nextBoundaryPos = content.find(boundary) + 1;
	// 	string nextContent = content.substr(nextBoundaryPos);
	// 	string endBoundary = boundary + "--";

	// 	while (true)
	// 	{
	// 		if (strncmp(nextContent.c_str(), ("-" + endBoundary).c_str(), endBoundary.size()) == 0)
	// 			break;
	// 		if (nextContent.find("filename=") != string::npos)
	// 		{
	// 			// extract filename
	// 			size_t filenamePos = nextContent.find("filename=");
	// 			filename = nextContent.substr(filenamePos + 10);
	// 			filename = filename.substr(0, filename.find("\""));

	// 			cout << "Filename: " << "|" + filename + "|" << endl;

	// 			// extract content
	// 			size_t dataPos = nextContent.find("\r\n\r\n") + 4;
	// 			size_t boundaryPosInData = nextContent.find("--" + boundary, dataPos);
	// 			string fileContent = nextContent.substr(dataPos, boundaryPosInData - dataPos);

	// 			// write into file
	// 			string	path = "./" + root + "/" + filename;
				
	// 			writeIntoFile(fileContent, path);
	// 		}
	// 		size_t nextBoundaryPos = nextContent.find("--" + boundary) + 1;
	// 		nextContent = nextContent.substr(nextBoundaryPos);
	// 	}

	// 	// send response if all data has been read
	// 	if (true)
	// 		return handlePostResponse();
	// 	else
	// 		return (0); // signify still have data
	// }

	int	Server::handlePostRequest()
	{
		string	root = "root"; // root dir
		string	headers = get_headers();
		string	content = get_content();
		string	file_content;

		// file stuff
		string	next_file_name;
		string	next_file_path;
		size_t	filename_index;

		// boundary variables
		string	boundary_string;
		size_t	boundary_string_index = headers.find("boundary=");

		boundary_string = headers.substr(boundary_string_index + 9);
		boundary_string = boundary_string.substr(0, boundary_string.find("\r"));

		// position of start / middle boundaries (looks like "--boundary_string")
		size_t	boundary_index = content.find("--" + boundary_string);

		// position of end boundary (looks like "--boundary_string--")
		size_t	boundary_end_index = content.find("--" + boundary_string + "--");

		// content type check is to ensure "filename=" before is not split up after chunking
		size_t	content_type_index = content.find("Content-Type:");

		size_t	data_start_index;

		// we handle stuff here
		if (boundary_index != string::npos && content_type_index != string::npos)
		{
			// if we see a filename it will become the next file to write to the next time the function is called
			filename_index = content.find("filename=", boundary_index);
			if (filename_index != string::npos)
				next_file_path = "./" + root + "/" + content.substr(filename_index + 10);

			data_start_index = content.find("\r\n\r\n", content_type_index);
			if (data_start_index != string::npos)
				file_content = content.substr(data_start_index + 4, content.length()); // start after \r\n\r\n

			content.clear();
			writeIntoFile(file_content, this->curr_post_file_path);
		}
		else if (boundary_end_index != string::npos)
		{
			this->status = DONE;
			file_content = content.substr(0, boundary_end_index - 4); // end before \r\n\r\n

			content.clear();
			writeIntoFile(file_content, this->curr_post_file_path);
		}

		if (next_file_path.empty() == false)
			this->curr_post_file_path = next_file_path;

		// send response if all data has been read
		if (this->status == DONE) // change this
			return handlePostResponse();
		else
			return (0); // signify still have data
	}

	int	Server::handlePostResponse()
	{
		string	response;

		this->status = DONE;
		response.append("HTTP/1.1 204 No Content\r\n\r\n");

		cout << "Header Sent: \n" << response << endl;
		return (sendData(this->newsocket, (void *) response.c_str(), response.length()));
	}

	// for content
	int	Server::readOnce()
	{
		int		bytes_read;
		char	buffer[BUFFER_SIZE];

		bytes_read = read(this->newsocket, buffer, sizeof(buffer));
		if (bytes_read == -1 || bytes_read == 0)
			return (-1);
		content.append(buffer);

		cout << "read once called" << endl;

		return (bytes_read);
	}
}