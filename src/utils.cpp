/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 16:50:57 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/20 13:47:52 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

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