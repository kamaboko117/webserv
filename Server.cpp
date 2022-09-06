#include <Server.hpp>

scfg::Server::Server() : _server_name(""), _listen(), _location()
{
	return ;
};

scfg::Server::~Server()
{
	return ;
};

std::string						scfg::Server::getServerName(void)
{
	return _server_name;
}

int								scfg::Server::getListen(void)
{
	return _listen;
}

scfg::Server::location			scfg::Server::getLocation(void)
{
	return _location;
}


void							scfg::Server::setServerName(std::string server_name)
{
	_server_name = server_name;
};

void							scfg::Server::setListen(int listen)
{
	_listen = listen;
};

void							scfg::Server::setLocation(location location)
{
	_location = location;
};