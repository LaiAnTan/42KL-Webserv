#include "Request.hpp"

using std::cout;
using std::endl;
using std::string;

namespace req
{
	Request::Request()
	{
		cout << "Request: Default constructor called" << endl;
	}

	Request::Request(std::ifstream *file)
	{
		cout << "Request: Constructor called" << endl;
		// do parsing here
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

	void	Request::updateHeaderField(std::string field_name, std::vector<std::string>	field_values)
	{
		// damn
	}

	std::ostream	&operator << (std::ostream outs, const Request &request)
	{

	}
}

