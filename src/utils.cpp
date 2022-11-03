/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 16:50:57 by asaboure          #+#    #+#             */
/*   Updated: 2022/11/03 17:14:55 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "Server.hpp"

bool canRead(const std::string &name){
    return !access(name.c_str(), R_OK);
}

bool existsFile (const std::string& name) {
    return !access(name.c_str(), F_OK);
}

int isDirectory(const std::string &path) {
   struct stat statbuf;
   if (stat(path.c_str(), &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

int ft_stoi(const std::string &s) {
    int i;
    std::istringstream(s) >> i;
    return i;
}

int ft_stol(const std::string &s) {
    long i;
    std::istringstream(s) >> i;
    return i;
}

char    *ft_strdupcpp(const char *src){
    std::size_t i = 0;

    while (src[i])
        i++;
    char    *ret = new char[i + 1];
    i = 0;
    while (src[i])
    {
        ret[i] = src[i];
        i++;
    }
    ret[i] = '\0';
    return(ret);
}

std::vector<cfg::t_location>::iterator        closestMatchingLocation(cfg::Server &server, std::string path)
{
    for (std::vector<cfg::t_location>::iterator it = server._locations.begin();
            it != server._locations.end(); ++it)
        if (it->_location == path)
            return it;
    for (size_t i = path.length(); i > 0; --i)
    {
        for (std::vector<cfg::t_location>::iterator it = server._locations.begin();
            it != server._locations.end(); ++it)
        {
            if (it->_location == path.substr(0, i))
                return it;
        }
    }
    return server._locations.end();
}
