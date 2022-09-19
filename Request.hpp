/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/19 14:03:50 by asaboure          #+#    #+#             */
/*   Updated: 2022/09/19 15:49:01 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string>
# include <map>

class Request
{
private:
    /*** GARBAGE FOR CANONICAL FORM NORM***/
    Request();
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    
    /*** private functions ***/
    void    initHeaders();

    /*** ETC ***/
    int                                 _method;
    std::string                         _body;
    std::map<std::string, std::string>  _headers;
public:
    Request(const std::string req);
    virtual ~Request();
};

Request::Request(/* args */)
{
}

Request::~Request()
{
}


#endif