#include "Server.hpp"
#include "../config/Config.hpp"
#include "../config/Config.hpp"
#include <sys/stat.h>

using std::cout;
using std::endl;

namespace HDE
{
	// extracts login details from header, returns a pair
	// gonna be depreciated in favour of a much more dynamic implementation
	// sorry william!

	// ...now that i think about this why is OH YEAHHH
	// COOL DEPRECIATED VALUE HEHHH
	// std::pair<string, string>	extract_login_details(string header)
	// {
	// 	size_t un_loc;
	// 	size_t pw_loc;
	// 	std::pair<string, string>	details;

	// 	un_loc = header.find("username=");
	// 	pw_loc = header.find("&password=");

	// 	if (un_loc == string::npos || pw_loc == string::npos) // if errror return empty pair
	// 	{
	// 		details.first = "";
	// 		details.second = "";
	// 	}
	// 	else
	// 	{
	// 		details.first = header.substr(un_loc + 9, pw_loc - (un_loc + 9));
	// 		details.second = header.substr(pw_loc + 10);
	// 	}
	// 	return (details);
	// }

	int Server::import_read_data()
	{
		int		bytesRead;
		char	buffer[BUFFER_SIZE];
		string	root = "root", filename;

		// more of a failsafe then anything
		// failsafe my fucking ass, if this happens its an error
		// this means THERES NO END BOUNDARY STRING
		if (this->content_length <= 0 && this->content.empty())
			return 1;
		// the end boundary string is found
		// and is located at the start of the buffer
		// this means all content successfully extracted

		// also, anything after the end boundary string is ignored
		if (this->content.find("--" + this->boundary_string + "--") == 0)
		{
			cout << YELLOW << "End Boundary String Found, Ending Process" << endl;
			this->status = CLEARING_SOCKET; // clear socket to send respond http message
			return 0;
		}

		if (this->content_length > 0)
		{
			bytesRead = read(this->newsocket, buffer, sizeof(buffer));
			if (bytesRead < 0)
			{
				std::cerr << RED << "Error when reading content" << endl;
				return 1;
			}
			this->content_length -= bytesRead;
			this->content.append(buffer, bytesRead);
		}

		if (this->content.find("--" + this->boundary_string) != string::npos)
		{
			cout << YELLOW << "Found a Boundary String" << endl;
			int boundary_pos = this->content.find("--" + this->boundary_string);

			// extract all previous data and put them into the old file
			string	previous_data = this->content.substr(0, boundary_pos);
			if (not previous_data.empty() && save_to.is_open())
			{
				save_to.write(previous_data.c_str(), previous_data.length());
				save_to.close();
			}
			this->content = this->content.substr(boundary_pos);

			// find the \r\n\r\n seperator between header and content
			// if dont have, just call function again 
			if (this->content.find("\r\n\r\n") == string::npos)
				return 0;

			// open another file
			// extract filename
			size_t	filenamePos = this->content.find("filename=");

			// if filename isnt read yet, just call this function again
			// actually dont need one since we already check that the seperator between the header and the content
			// (\r\n\r\n) is present
			if (filenamePos == string::npos)
				return 0;

			filename = this->content.substr(filenamePos + 10);
			size_t	filenameEnd = filename.find("\r\n");

			// if filename is chopped, just recall the function again
			// actually dont need one since we already check that the seperator between the header and the content
			// (\r\n\r\n) is present
			if (filenameEnd == string::npos)
				return 0;

			filename = filename.substr(0, filename.find("\""));
			cout << "Filename: " << "|" + filename + "|" << endl;

			string	path = "./" + root + "/" + filename;
			save_to.open(path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

			// once done, jump to content
			// jesus christ william this better be correct
			size_t	dataPos = this->content.find("\r\n\r\n") + 4;
			this->content = this->content.substr(dataPos);

			// DO NOT put data into the new file YET, as there MAY BE potential boundary strings
			return 0;
		}
		// there might be a potential boundary string
		// check for potential boundary strings
		else if (this->content.rfind("\r\n--") != string::npos)
		{
			cout << YELLOW << "Potential Boundary String Found" << endl;
			int	delimiter_pos = this->content.rfind("\r\n--");

			if (this->boundary_string.find(this->content.substr(delimiter_pos)) != string::npos)
			{
				// may or may not be a delimiter string, do not put this in just in case

				// just call this function again to read more data and determine next move
				return 0;
			}
		}
		// no potential boundary strings, no boundary strings found, nothing
		// it is safe to just dump data
		else
		{
			cout << YELLOW << "Dumping Data" << endl;
			this->save_to.write(this->content.c_str(), this->content.length());
			this->content.clear();
			return 0;
		}
		return 0;
	}

	int	Server::handlePostRequest()
	{
		string	root = "root";
		string	boundary, filename, path;
		size_t	boundaryPos = headers.find("boundary=");
		
		path = headers.substr(headers.find("POST ") + 5);
		path = path.substr(0, path.find(" "));

		this->boundary_string = headers.substr(boundaryPos + 9);
		this->boundary_string = boundary_string.substr(0, boundary_string.find("\r"));

		cout << YELLOW << "Handling Post Request" << endl;

		// handle client_max_body_size
		double		limit;
		double		converted;
		string		client_max_body_size;
		string		suffix;

		std::map<string, conf::ServerLocation>	location = this->config->get_locations();

		client_max_body_size = location[location_config_path].get_client_max_body_size();
		if (client_max_body_size.empty())
			client_max_body_size = config->get_client_max();

		if (not client_max_body_size.empty())
		{
			suffix = client_max_body_size.substr(client_max_body_size.size() - 2);
			if (not (suffix == "KB" || suffix == "MB" || suffix == "GB"))
			{
				if (client_max_body_size.substr(client_max_body_size.size() - 1) == "B")
					suffix = client_max_body_size.substr(client_max_body_size.size() - 1);
				else // will never happen hopefully
					throw (conf::InvalidSuffixException()); // i do love crashing my server whenever the config suffix is wrong
			}

			limit = std::strtof(client_max_body_size.substr(0, client_max_body_size.find(suffix)).c_str(), NULL);
			converted = convert_content_length(suffix);
		}
		else
		{
			limit = -1;
			converted = this->content_length;
		}

		if (limit > 0 && converted > limit)
		{
			cout << "Over the limit " << converted << " > " << limit << endl;
			this->error_code = "413";
			this->status = CLEARING_SOCKET; // clear socket to send error message
		}
		else
		{
			if (limit == -1)
				cout << "[NOTICE] No Limit" << endl;
			else
				cout << "[NOTICE] Within Limit" << endl;
			this->status = SAVE_CHUNK;
			// wow nothing is posted
			if (this->content_length == 0){
				this->status = SENDING_RESPONSE;
			}
		}
		return 0;
	}

	int	Server::handlePostResponse()
	{
		string	response;

		this->status = DONE;
		response.append("HTTP/1.1 204 No Content\r\n\r\n");

		cout << "Header Sent: \n" << response << endl;
		return sendData(this->newsocket, (void *) response.c_str(), response.length());
	}
}