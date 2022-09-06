#include <Server.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>

// typedef		std::string	(*getValuesOfKey)(std::vector<std::string>);

// const		std::map<std::string, getValuesOfKey>	parsing
// {
// 	std::make_pair("server", )
// }

void					matchingBrackets(std::vector<std::string> cfg)
{
	int		ob = 0;
	int		cb = 0;

	for (std::vector<std::string>::iterator it = cfg.begin();
			it != cfg.end(); it++)
	{
		for (size_t i = 0; i < it->length(); i++)
		{
			if (it->at(i) == '{')
				ob++;
			else if (it->at(i) == '}')
				cb++;
		}
	}
	if (ob != cb)
		std::cerr << "Brackets are not matching !" << std::endl;
}

typedef	std::vector<std::pair<std::vector<std::string>::iterator, std::vector<std::string>::iterator> > t_context;

t_context		*getServerContext(std::vector<std::string> cfg)
{
	t_context		*context = new t_context;
}

void					parseCfgFile(std::string cfg_file_name)
{
	std::vector<std::string>		cfg_file_content;
	std::ifstream					cfg_file;
	std::string						buffer;

	cfg_file.open(cfg_file_name.c_str());
	if (cfg_file.is_open() == false)
		throw ("Couldn't open file: " + cfg_file_name + " !");
	while (cfg_file.eof() == false)
	{
		std::getline(cfg_file, buffer);
		cfg_file_content.push_back(buffer);
	}
	matchingBrackets(cfg_file_content);
}

scfg::Server			*getServers(std::string cfg_file_name)
{
	parseCfgFile(cfg_file_name);

	scfg::Server		*servers = new scfg::Server;
	return servers;
}

int		main(int ac, char **av)
{
	if (ac != 2)
		return 1;
	getServers(av[1]);
}