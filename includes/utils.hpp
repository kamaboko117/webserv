/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 16:52:42 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/20 13:42:17 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP
# include <string>

bool    existsFile (const std::string &);
int     ft_stoi(const std::string &);
long    ft_stol(const std::string &);
char    *ft_strdupcpp(const char *);
int     isDirectory(const std::string &);
bool    canRead(const std::string &);

#endif