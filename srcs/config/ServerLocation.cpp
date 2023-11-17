#include "Config.hpp"
#include <set>

namespace conf
{
	void ServerLocation::set_root(string text)
	{
		// cout << "---root---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");
		
		if (tokens.size() != 2 || tokens[0] != "root")
			throw (conf::TooManyValuesException());

		this->root = tokens[1];
	}

	void	ServerLocation::set_index(string text)
	{
		// cout << "---index---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 2 || tokens[0] != "index")
			throw (conf::TooManyValuesException());

		this->index = tokens[1];
	}

	void	ServerLocation::set_autoindex(string text)
	{
		// cout << "---autoindex---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 2 || tokens[0] != "autoindex")
			throw (conf::TooManyValuesException());

		this->autoindex = tokens[1];
	}

	void	ServerLocation::set_client_max_body_size(string text)
	{
		// cout << "---client_max_body_size---" << endl;
		// cout << "---client_max---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 2 || tokens[0] != "client_max_body_size")
			throw (conf::TooManyValuesException());

		string res = tokens[1];

		string suffix = res.substr(res.size() - 2);

		if (not (suffix == "KB" || suffix == "MB" || suffix == "GB"))
		{
			if (res.substr(res.size() - 1) == "B")
				suffix = res.substr(res.size() - 1);
			else
				throw (conf::InvalidSuffixException());
		}

		char *end = NULL;

		cout << res.substr(0, res.find(suffix)).c_str() << endl;
		std::strtof(res.substr(0, res.find(suffix)).c_str(), &end);
		if (*end != '\0')
			throw (conf::InvalidValueException());

		this->client_max_body_size = res;
	}

	void	ServerLocation::set_return_path(string text)
	{
		// cout << "---return_path---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() != 2 || tokens[0] != "return")
			throw (conf::TooManyValuesException());

		this->return_path = tokens[1];
	}

	void	ServerLocation::set_allowed_method(string text)
	{
		// cout << "---allowed_method---" << endl;
		std::vector<string>		tokens;

		tokens = util::split_many_delims(text, " \t");

		if (tokens.size() < 2 || tokens[0] != "allowed_methods")
			throw (conf::TooManyValuesException());

		tokens.erase(tokens.begin());
		this->allowed_method = tokens;
	}

	string	ServerLocation::get_root() const
	{
		return (this->root);
	}

	string	ServerLocation::get_index() const
	{
		return (this->index);
	}

	string	ServerLocation::get_autoindex() const
	{
		return (this->autoindex);
	}

	string	ServerLocation::get_client_max_body_size() const
	{
		return (this->client_max_body_size);
	}

	string	ServerLocation::get_return_path() const
	{
		return (this->return_path);
	}

	const std::vector<string>	&ServerLocation::get_allowed_method() const
	{
		return (this->allowed_method);
	}

	ServerLocation::ServerLocation()
	{}

	ServerLocation::ServerLocation(std::ifstream *file)
	{
		int i;		
		size_t		semicolon_pos;
		size_t		comment_pos;
		string text, var1, var2, key, value;
		// std::vector<string> tmp, value_vec;

		valid_keywords.insert("root");
		valid_keywords.insert("index");
		valid_keywords.insert("allowed_methods");
		valid_keywords.insert("client_max_body_size");
		valid_keywords.insert("autoindex");
		valid_keywords.insert("return");
		valid_keywords.insert("alias");

		void (ServerLocation::*funct[])(string text) = {
			&ServerLocation::set_root,
			&ServerLocation::set_autoindex,
			&ServerLocation::set_allowed_method,
			&ServerLocation::set_client_max_body_size,
			&ServerLocation::set_index,
			&ServerLocation::set_return_path
		};

		string arr[] = {"root", "autoindex", "allowed_methods", \
			"client_max_body_size", "index", "return"};

		// set defaults in case they are not found
		std::vector<string>		empty;

		this->allowed_method = empty;
		this->root = "";
		this->index = "";
		this->client_max_body_size = "";
		this->autoindex = "on";
		this->return_path = "";

		while (std::getline(*file, text))
		{
			if (text[1] == '}')
				break;
			if ((comment_pos = text.find('#')) != std::string::npos) // remove comments
				text.resize(comment_pos);
			if (text.find_first_not_of(" \t\n\v\f\r") == std::string::npos) // handle blank lines
				continue;
			else if ((semicolon_pos = text.find(";")) == string::npos)
				throw (conf::MissingSemicolonException());
			i = 0;
			while (i < 6 && text.find(arr[i]) == std::string::npos)
				i++;
			if (i < 6)
			{
				if ((semicolon_pos = text.find(";")) == string::npos)
					throw (conf::MissingSemicolonException());
				text.resize(semicolon_pos);
				(this->*funct[i])(text);
			}
		}
		if (conf::validateKeywords(valid_keywords, rules) == false)
			throw (conf::InvalidKeywordException());
	}


	ServerLocation::ServerLocation(const ServerLocation &L)
	{
		*this = L;
	}

	ServerLocation	&ServerLocation::operator=(const ServerLocation &L)
	{
		if (this != &L)
		{
			this->root = L.root;
			this->index = L.index;
			this->autoindex = L.autoindex;
			this->client_max_body_size = L.client_max_body_size;
			this->return_path = L.return_path;
			this->allowed_method = L.allowed_method;
			this->rules = L.rules;
		}
		return (*this);
	}

	ServerLocation::~ServerLocation()
	{
	}

	const std::map<string, std::vector<string> > &ServerLocation::get_rules() const
	{
		return(this->rules);
	}

	std::ostream &operator << (std::ostream &outs, const ServerLocation &server_location)
	{
		if(!server_location.get_root().empty())
			outs << YELLOW "Root : " RESET << server_location.get_root() << endl;
		if (!server_location.get_index().empty())
			outs << YELLOW "Index : " RESET << server_location.get_index() << endl;
		if (!server_location.get_autoindex().empty())
			outs << YELLOW "Autoindex : " RESET << server_location.get_autoindex() << endl;
		if (!server_location.get_client_max_body_size().empty())
			outs << YELLOW "Client_max_body_size : " RESET << server_location.get_client_max_body_size() << endl;
		if (!server_location.get_return_path().empty())
			outs << YELLOW "Return : " RESET << server_location.get_return_path() << endl;
		if (!server_location.get_allowed_method().empty())
		{
			outs << YELLOW "Allowed Methods : " RESET;
			std::vector<string> allowed_methods = server_location.get_allowed_method();
			std::vector<string>::iterator it, end = allowed_methods.end();
			for (it = allowed_methods.begin(); it != end; ++it)
				outs << *it << " ";
			outs << endl;
		}
		return (outs);
	}
}