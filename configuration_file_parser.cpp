#include <Server.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <list>
#include <stdio.h>

#define	NPOS std::string::npos
#define SET ""

std::list<std::string>		*split(std::string str, std::string delimiter)
{
	size_t						begin = 0;
	size_t						end = 0;
	std::list<std::string>		*sp = new std::list<std::string>;
	std::string					buff;

	while (end != NPOS)
	{
		end = str.find(delimiter, begin);
		buff = str.substr(begin, end - begin);
		if (buff.length() > 0)
			sp->push_back(buff);
		begin = end + delimiter.length();
	}
	return sp;
}

bool						findKeyWithBrace(std::string str, std::string key, std::list<std::string> &servers)
{
	size_t							i = 0;
	size_t							tmp;

	for (; i < str.length(); i++)
	{
		for (; !isalpha(str[i]) && i < str.length();)
		{
			for (; isspace(str[i]) && i < str.length(); i++);
			if (str[i] == '#')
				for (; str[i] != '\n' && i < str.length(); i++);
		}
		for (size_t j = 0; i < str.length() && j < key.length(); i++, j++)
			if (str[i] != key[j])
				return false;
		for (; isspace(str[i]) && i < str.length(); i++);
		if (key == "location")
		{
			if (str[i] == '.')
				i++;
			for (; (isalnum(str[i]) || str[i] == '/') && i < str.length(); i++);
		}
		if (str[i] != '{')
			return false;
		if (i < str.length())
			i++;
		size_t size = 0;
		tmp = i;
		for (size_t flag = 1; i < str.length() && flag != 0; i++)
		{
			if (str[i] == '{')
				flag++;
			else if (str[i] == '}')
				flag--;
			if (flag != 0)
				size++;
		}
		servers.push_back(str.substr(tmp, size));
	}
	return (servers.size()) ? true : false;
}

bool						findKeyWithSemicolon(std::string &str, std::string key, std::list<std::string> &attributes)
{
	size_t							i;
	size_t							semicolonPos = NPOS;
	std::string						tmp;
	std::list<std::string>			*sp_tmp;

	if ((i = str.find(key, 0)) == NPOS)
		return false;
	semicolonPos = str.find(';', i);
	if (str.find('\n', i) < semicolonPos)
		return false;
	tmp = str.substr(i, semicolonPos - i);
	str.erase(i, semicolonPos - i);
	sp_tmp = split(tmp, " ");
	attributes = *sp_tmp;
	delete sp_tmp;
	return (attributes.size()) ? true : false;
}

std::string		*getFileContent(char *file_name)
{
	std::string					*file_content = new std::string;
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
		buffer.append("\n");
		file_content->append(buffer);
	}
	return file_content;
}

void			getServers(char *file_name, std::vector<cfg::Server> &servers)
{
	std::string						*file_content = getFileContent(file_name);
	std::list<std::string>			raw_servers;
	std::list<std::string>			raw_locations;

	if (findKeyWithBrace(*file_content, "server", raw_servers) == false)
	{
		delete file_content;
		return ;
	}
	for (std::list<std::string>::iterator it = raw_servers.begin(); it != raw_servers.end(); it++)
	{
		
	}
}

int		main(int ac, char **av)
{
}