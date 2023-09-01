#include "Respond.hpp"
#include "Utils.hpp"

namespace resp
{
	Respond::Respond()
	{
		cout << "Respond default constructor called" << endl;
	}
	
	Respond::~Respond()
	{
		cout << "Respond default destructor called" << endl;
	}

	//getter
	string	Respond::getRequestURL()
	{
		return(this->req_url);
	}
	string	Respond::getRequestMethod()
	{
		return(this->req_method);
	}
	string	Respond::getCode()
	{
		return(this->code);
	}
	string	Respond::getContentType()
	{
		return(this->content_type);
	}

	//setter
	void	Respond::setRequestURL(string var)
	{
		this->req_url = var;
	}
	void	Respond::setRequestMethod(string var)
	{
		this->req_method = var;
	}
	void	Respond::setCode(string var)
	{
		this->code = var;
	}
	void	Respond::setContentType(string var)
	{
		this->content_type = var;
	}
}