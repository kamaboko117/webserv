#include <Server.hpp>
#include <utils.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <list>
#include <vector>
#include <stdio.h>
#include <ctype.h>
#include <map>
#include <sstream>

#define	NPOS std::string::npos
#define SET ""
typedef	void (*t_getters)(std::vector<std::string>::iterator &, std::vector<std::string>::iterator, std::vector<cfg::Server> &);
typedef struct		s_keywords
{
	std::string		keyword;
	unsigned char	depth;
}					t_keywords;

struct ParseError : public std::exception {
	const char * what () const throw ()
	{
		return "Parse error !";
	}
};

struct FileError : public std::exception {
	const char * what () const throw ()
	{
		return "Couldn't open file !";
	}
};

void						skipToChar(std::string str,  size_t &pos, char c)
{
	if ((pos = str.find(c, pos)) == NPOS)
		throw ParseError();
}

template<class UnaryPredicate>
void						whilePred(std::string str, size_t &pos, UnaryPredicate p)
{
	while (pos < str.length() && p(str[pos]))
		++pos;
}

template<class UnaryPredicate>
void						whileNotPred(std::string str, size_t &pos, UnaryPredicate p)
{
	while (pos < str.length() && !p(str[pos]))
		++pos;
}

int							sstoi(std::string str)
{
	int						i;
	size_t					tmp = 0;

	whilePred(str, tmp, isdigit);
	if (tmp != str.length())
		throw ParseError();
	std::istringstream(str) >> i;
	return i;
}

void						skipQuotes(std::string str, size_t &pos)
{
	int						flag = 1;
	char					type_of_quote = str[pos];

	++pos;
	while (pos < str.length() && flag != 0)
	{
		if (str[pos] == type_of_quote)
			flag--;
		++pos;
	}
	if (pos == str.length() || flag != 0)
		throw ParseError();
}

std::string			getFileContent(char *file_name)
{
	std::string					buffer;
	std::ifstream				file;

	file.open(file_name, std::ios_base::in);
	try
	{
		file.exceptions(std::ifstream::failbit);
	}
	catch (std::exception &e)
	{
		throw FileError();
	}
	std::string file_content((std::istreambuf_iterator<char>(file)),
					(std::istreambuf_iterator<char>()));
	return file_content;
}

std::vector<std::string>	stripCfg(std::string str)
{
	size_t						pos = 0;
	size_t						begin = 0;
	std::string					tmp;
	std::vector<std::string>	strippedCfg;

	while (pos < str.length())
	{
		if (isspace(str[pos]))
				whilePred(str, pos, isspace);
		else if (str[pos] == '#')
				skipToChar(str, pos, '\n');
		else if (str[pos] == ';' || str[pos] == '{' || str[pos] == '}')
		{
			strippedCfg.push_back(str.substr(pos, 1));
			++pos;
		}
		else if (isprint(str[pos]))
		{
			begin = pos;

			if (str[pos] == '\'' || str[pos] == '\"')
			{
				skipQuotes(str, pos);
				strippedCfg.push_back(str.substr(begin + 1, pos - begin - 2));
			}
			else
			{
				while (pos < str.length() && str[pos] != ';' && str[pos] != '{' && str[pos] != '}' && !isspace(str[pos]))
					++pos;
				strippedCfg.push_back(str.substr(begin, pos - begin));
			}
		}
		else
			throw ParseError();
	}
	return strippedCfg;
}

void							getServer
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	(void)pos;
	(void)end;
	server.push_back(cfg::Server());
	server.back()._listen = 0;
}

void							getListen
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	int					tmp = -42;

	++pos;
	if (pos == end)
		throw ParseError();
	size_t				i = pos->find(':', 0);
	if (i == NPOS)
		tmp = sstoi(*pos);
	else
		tmp = sstoi(pos->substr(i + 1));
	server.back()._listen = tmp;
	++pos;
}

void						getServerName
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	std::vector<std::string>::iterator		begin;

	++pos;
	begin = pos;
	while (pos != end && *pos != ";")
	{
		server.back()._server_name.push_back(*pos);
		++pos;
	}
}

void                        getErrorPage
(
    std::vector<std::string>::iterator        &pos,
    std::vector<std::string>::iterator        end,
    std::vector<cfg::Server>                &server
)
{
    int            tmp;

    ++pos;
    if (pos == end || pos + 1 == end)
        throw ParseError();
    tmp = sstoi(*pos);
    ++pos;
    server.back()._error_page[tmp] = *pos;
    ++pos;
}

void						getClientMaxBodySize
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	int					tmp = -42;

	++pos;
	if (pos == end)
		throw ParseError();
	if ((tmp = sstoi(*pos)) < 0)
		throw ParseError();
	server.back()._client_max_body_size = tmp;
	++pos;
}

void						getLocation
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	++pos;
	if (pos == end)
		throw ParseError();
	server.back()._locations.push_back(cfg::t_location());
	server.back()._locations.back()._location = *pos;
	server.back()._locations.back()._autoindex = false;
}

bool						isAllowedMethod(std::string m)
{
	char const	*methods[] = {"GET", "POST", "DELETE", NULL};

	for (int i = 0; methods[i]; ++i)
		if (m == std::string(methods[i]))
			return true;
	return false;
}

void						getAllow
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{

	++pos;
	while (pos != end && *pos != ";")
	{
		if (!isAllowedMethod(*pos))
			throw ParseError();
		server.back()._locations.back()._allow.push_back(*pos);
		++pos;
	}
}

void						getAutoindex
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	++pos;
	if (pos == end)
		throw ParseError();
	if (*pos == "on")
		server.back()._locations.back()._autoindex = true;
	else if (*pos == "off")
		server.back()._locations.back()._autoindex = false;
	else
		throw ParseError();
	++pos;
}

void					getReturn
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	std::pair<int, std::string>				ret;

	++pos;
	if (pos == end)
		throw ParseError();
	ret.first = sstoi(*pos);
	++pos;
	if (pos == end)
		throw ParseError();
	ret.second = *pos;
	server.back()._locations.back()._return = ret;
	++pos;
}

void					getRoot
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	++pos;
	if (pos == end)
		throw ParseError();
	if (*(pos->rbegin()) != '/')
		pos->append("/");
	server.back()._locations.back()._root = *pos;
	++pos;
}

void					getIndex
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	++pos;
	while (pos != end && *pos != ";")
	{
		server.back()._locations.back()._index.push_back(*pos);
		++pos;
	}
}

void					getCgipass
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	++pos;
	if (pos == end || pos + 1 == end)
		throw ParseError();
	server.back()._locations.back()._cgi_pass[*pos] = *(pos + 1);
	pos += 2;
}

void					getUploadStore
(
	std::vector<std::string>::iterator		&pos,
	std::vector<std::string>::iterator		end,
	std::vector<cfg::Server>				&server
)
{
	++pos;
	if (pos == end)
		throw ParseError();
	// if (!isDirectory(*pos))
	// 	throw ParseError();
	server.back()._locations.back()._upload_store = *pos;
	++pos;
}

t_keywords const			keywords[] =
{
	{"server", 0},
	{"listen", 1},
	{"server_name", 1},
	{"error_page", 1},
	{"client_max_body_size", 1},
	{"location", 1},
	{"allow", 2},
	{"autoindex", 2},
	{"return", 2},
	{"root", 2},
	{"index", 2},
	{"cgi_pass", 2},
	{"upload_store", 2},
	{"", 0}
};

bool						isKeywordAndGoodDepth(std::string str, int depth)
{
	size_t					i = 0;

	while (!keywords[i].keyword.empty())
	{
		if (str == keywords[i].keyword && keywords[i].depth == depth)
			return true;
		++i;
	}
	return false;
}

t_getters						getKeyWordFunc(std::string keyword)
{
	static		std::map<std::string, t_getters>		getters;

	if (!getters.empty())
		return getters[keyword];
	getters["server"] = getServer;
	getters["listen"] = getListen;
	getters["server_name"] = getServerName;
	getters["error_page"] = getErrorPage;
	getters["client_max_body_size"] = getClientMaxBodySize;
	getters["location"] = getLocation;
	getters["allow"] = getAllow;
	getters["autoindex"] = getAutoindex;
	getters["return"] = getReturn;
	getters["root"] = getRoot;
	getters["index"] = getIndex;
	getters["cgi_pass"] = getCgipass;
	getters["upload_store"] = getUploadStore;
	return getters[keyword];
}

std::vector<cfg::Server>					getServers(char *filename)
{
	std::vector<std::string>				cfg = stripCfg(getFileContent(filename));
	std::vector<cfg::Server>				servers;
	std::vector<std::string>::iterator		pos = cfg.begin();
	int										depth = 0;

	while (pos != cfg.end())
	{
		if (isKeywordAndGoodDepth(*pos, depth))
			getKeyWordFunc(*pos)(pos, cfg.end(), servers);
		else if (*pos == "{")
			++depth;
		else if (*pos == "}")
			--depth;
		else if ((*pos != ";" && pos != cfg.end()) || (pos == cfg.end() && depth != 0))
			throw ParseError();
		if (pos != cfg.end())
			++pos;
	}
	if (pos == cfg.end() && depth != 0)
		throw ParseError();
	return servers;
}

// void	printCfg(std::vector<cfg::Server> s)
// {
// 	for (auto serverIt = s.begin(); serverIt != s.end(); ++serverIt)
// 	{
// 		std::cout << "listen: " << serverIt->_listen << std::endl;
// 		for (auto serverNameIt = serverIt->_server_name.begin(); serverNameIt != serverIt->_server_name.end(); ++serverNameIt)
// 			std::cout << "server_name: " << *serverNameIt << std::endl;
// 		std::cout << "client_max_body_size: " << serverIt->_client_max_body_size << std::endl;
// 		for (auto serverErrorPageIt = serverIt->_error_page.begin(); serverErrorPageIt != serverIt->_error_page.end(); ++serverErrorPageIt)
// 			std::cout << "error_page: " << serverErrorPageIt->first << " | " << serverErrorPageIt->second << std::endl;
// 		for (auto serverLocationsIt = serverIt->_locations.begin(); serverLocationsIt != serverIt->_locations.end(); ++serverLocationsIt)
// 		{
// 			std::cout << "location: " << serverLocationsIt->_location << std::endl;
// 			for (auto locationAllowIt = serverLocationsIt->_allow.begin(); locationAllowIt != serverLocationsIt->_allow.end(); ++locationAllowIt)
// 				std::cout << "allow: " << *locationAllowIt << std::endl;
// 			std::cout << "return: " << serverLocationsIt->_return.first << " | " << serverLocationsIt->_return.second;
// 			std::cout << "root: " << serverLocationsIt->_root << std::endl;
// 			std::cout << "autoindex: " << serverLocationsIt->_autoindex << std::endl;
// 			for (auto locationIndexIt = serverLocationsIt->_index.begin(); locationIndexIt != serverLocationsIt->_index.end(); ++locationIndexIt)
// 				std::cout << "index: " << *locationIndexIt << std::endl;
// 			for (auto locationCgipassIt = serverLocationsIt->_cgi_pass.begin(); locationCgipassIt != serverLocationsIt->_cgi_pass.end(); ++locationCgipassIt)
// 				std::cout << locationCgipassIt->first << " | " << locationCgipassIt->second << std::endl;
// 			std::cout << "upload_store: " << serverLocationsIt->_upload_store << std::endl;
// 		}
// 		std::cout << "-------------------------------" << std::endl;
// 	}
// }

// int main(int ac, char *av[])
// {
// 	(void)ac;
// 	std::vector<cfg::Server>		s = getServers(av[1]);
// 	printCfg(s);
// }