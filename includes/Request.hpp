/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/19 14:03:50 by asaboure          #+#    #+#             */
/*   Updated: 2022/09/29 15:50:05 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP
# include <string>
# include <map>
# include <vector>

class Request
{
private:
    /*** GARBAGE FOR CANONICAL FORM NORM***/
    Request();
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    
    /*** private functions ***/
    void                        initHeaders();
    void                        initMethods();
    void                        setPath(std::string, std::size_t);
    void                        setBody(std::size_t, std::vector<std::string>);
    std::vector<std::string>    reqSplit(std::string);

    /*** ETC ***/
    std::string                         _method;
    std::vector<std::string>            _methods;
    std::string                         _path;
    std::string                         _version;
    std::string                         _body;
    std::map<std::string, std::string>  _headers;
    int                                 _ret;
public:
    Request(const std::string req);
    virtual ~Request();

    std::string                         getPath() const;
    std::map<std::string, std::string>  getHeaders() const;
    std::string                         getMethod() const;
    std::string                         getBody() const;
};

#endif