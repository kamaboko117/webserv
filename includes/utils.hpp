/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/27 16:52:42 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/05 16:29:47 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP
# include <string>

bool    existsFile (const std::string &);
int     ft_stoi(const std::string &);
long    ft_stol(const std::string &);
char    *ft_strdupcpp(const char *);

#endif