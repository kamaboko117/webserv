/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 16:50:57 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/05 16:28:48 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <fstream>
#include <sstream>

bool existsFile (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
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