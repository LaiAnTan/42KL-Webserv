#include "Request.hpp"
#include "Utils.hpp"

using std::cout;
using std::endl;

namespace req
{
	Request::Request()
	{
		cout << "Request: Default constructor called" << endl;
	}

	Request::Request(std::ifstream *file)
	{
		cout << "Request: Constructor called" << endl;
		
		std::string					line;
		std::string					key;
		std::string					val;
		std::string					delim;
		std::vector<std::string>	tokens;
		std::vector<std::string>	valueVec;

		// request line
		std::getline(*file, line);
		tokens = util::split(line, " ");
		if (tokens.size() != 3)
			throw (Request::WrongRequestFormatException());
		this->method = tokens[0];
		this->target = tokens[1];
		this->ver = tokens[2];
		tokens.clear();

		// request header
		while (std::getline(*file, line))
		{
			tokens = util::split(line, ":");
			key = tokens[0];
			val = tokens[1];
			tokens.clear();

			// handle values
			if (key == "User-Agent")
				delim = " ";
			else
				delim = ",";

			valueVec = util::split(val, delim);
			updateHeaderField(key, valueVec);
		}
	}

	Request::Request(const Request &request)
	{
		cout << "Request: Copy constructor called" << endl;
		*this = request;
	}

	Request &Request::operator = (const Request &request)
	{
		cout << "Request: Copy assignment operator called" << endl;
		if (this == &request)
			return (*this);
		this->method = request.method;
		this->target = request.target;
		this->ver = request.ver;
		this->header = request.header;
		return (*this);
	}

	Request::~Request()
	{
		cout << "Request: Destructor called" << endl;
	}

	std::string	Request::getMethod(void) const
	{
		return (this->method);
	}

	std::string	Request::getTarget(void) const
	{
		return (this->target);
	}

	std::string	Request::getVersion(void) const
	{
		return (this->ver);
	}

	const Request::requestHeader	&Request::getRequestHeader(void) const
	{
		return (this->header);
	}

	void	Request::setMethod(std::string method)
	{
		this->method = method;
	}

	void	Request::setTarget(std::string target)
	{
		this->target = target;
	}

	void	Request::setVersion(std::string version)
	{
		this->ver = version;
	}

	void	Request::updateHeaderField(std::string field_name, std::vector<std::string> field_values)
	{
		std::vector<std::string> 	curr_values;
		requestHeader::iterator		it = this->header.find(field_name);
	
		if (it == this->header.end())
			this->header.insert(std::make_pair(field_name, field_values));
		else
		{
			curr_values = this->header[field_name];
			curr_values.insert(curr_values.end(), field_values.begin(), field_values.end());
			this->header[field_name] = curr_values;
		}
	}

	void	Request::updateHeaderField(std::string field_name, std::string field_value)
	{
		std::vector<std::string> 	curr_values;
		std::vector<std::string>	new_values(1, field_value);
		requestHeader::iterator		it = this->header.find(field_name);
	
		if (it == this->header.end())
			this->header.insert(std::make_pair(field_name, new_values));
		else
		{
			curr_values = this->header[field_name];
			curr_values.insert(curr_values.end(), new_values.begin(), new_values.end());
			this->header[field_name] = curr_values;
		}
	}

	const char* Request::WrongRequestFormatException::what() const throw()
	{
		return ("WrongRequestFormatException: Request in wrong format");
	}

	std::ostream	&operator << (std::ostream outs, const Request &request)
	{

	}
}

