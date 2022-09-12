#include <Server.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>

std::vector<std::string>		*getFileContent(char *file_name)
{
	std::vector<std::string>	*file_content = new std::vector<std::string>;
	std::string					buffer;
	std::ifstream				file;

	try
	{
		file.open(file_name, std::ios_base::in);
		file.exceptions(std::ifstream::failbit);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return NULL;
	}
	while (!file.eof())
	{
		std::getline(file, buffer);
		file_content->push_back(buffer);
	}
	return file_content;
}

std::vector<cfg::Server>	*getServersCfg(std::vector<std::string> file_content)
{
	
}

int		main(int ac, char **av)
{
	(void)av;
	if (ac != 2)
		return 1;
	std::vector<std::string>	*file_content = getFileContent(av[1]);
	for (std::vector<std::string>::iterator it = file_content->begin(); it != file_content->end(); it++)
		std::cout << *it << std::endl;
}