# include "Server.hpp"
# include <iostream>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <time.h>
# include <iomanip>

using std::cout;
using std::endl;

namespace HDE
{
	// always remember to encode the url to your file...
	string	Server::encode_url(const string &value)
	{
		std::stringstream	save;

		// frankly? im lazy to redo this so
		// credit: https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
		for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
			string::value_type c = (*i);

			// Keep alphanumeric and other accepted characters intact
			// also the / since, well, url :P
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '/') {
				save << c;
				continue;
			}

			// Any other characters are percent-encoded
			save << std::uppercase;
			save << '%' << std::setw(2) << std::hex << int((unsigned char) c);
			save << std::nouppercase;
		}
		return save.str();
	}

	int	Server::generate_index()
	{
		std::stringstream	index_content;
		std::vector<string>	file_name;
		std::vector<string>	file_date;
		std::vector<int>	file_size;

		struct stat		buf;
		int				exist;
		DIR				*d;
		struct dirent	*de;

		struct tm			*timeinfo;

		// there is no way the length of the date is more than 50
		char			save_buffer[50];

		index_content << "<!DOCTYPE html>" << endl;
		index_content << "<html>" << endl;
		index_content << "<style>" << endl;
		index_content << "table, th, td {\nborder:1px solid black;\n}" << endl;
		index_content << "</style>" << endl;
		index_content << "<body>" << endl;
		index_content << "<h2> Index of " << this->path << "</h2>" << endl;
		index_content << "<table style=\"width:100%\">" << endl;
		index_content << "<tr><th>Name</th><th>Date</th><th>Size</th></tr>" << endl;

		cout << this->real_filepath << endl;
		d = opendir(this->real_filepath.c_str());
		string	full_file_path, domain_file_path, encoded_file_path;
		for (de = readdir(d); de != NULL; de = readdir(d))
		{
			full_file_path = this->real_filepath + de->d_name;
			domain_file_path = this->path + "/" + de->d_name;
			exist = stat(full_file_path.c_str(), &buf);
			if (string(de->d_name) == ".")
				continue;
			if (exist < 0)
			{
				cout << "No exist file" << endl;
			} else
			{
				index_content << "<tr>" << endl;
				encoded_file_path = encode_url(domain_file_path);
				// handled name
				index_content << "<td><a href=\"" << encoded_file_path << "\">" << de->d_name << "</a></td>" << endl;
				// handles date
				timeinfo = localtime(&(buf.st_mtim.tv_sec));
				strftime(save_buffer, 100, "%e-%B-%Y", timeinfo);
				index_content << "<td>" << save_buffer << "</td>" << endl;

				// handles size
				index_content << "<td>" << buf.st_size << "</td>" << endl;

				index_content << "</tr>" << endl;
			}
		}

		index_content << "</table>" << endl;
		index_content << "</body></html>" << endl;
		std::stringstream	to_send;

		to_send << "HTTP/1.1 200 OK\r\n" << "Connection: keep-alive\r\n" << "Content-Type: text/html\r\n" << "Content-Length: " << index_content.str().length() << "\r\n\r\n";
		to_send << index_content.str().c_str();

		return sendData(this->newsocket, to_send.str().c_str(), to_send.str().length());
	}
}