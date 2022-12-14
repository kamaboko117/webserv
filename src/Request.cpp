/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/19 14:20:16 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/10 17:58:05 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <vector>
#include <iostream>

Request::Request(const std::string req)
: _method(""), _path(""), _version(""), _body(""), _ret(200){
    initHeaders();
    initMethods();
    std::vector<std::string> line = reqSplit(req);
    std::string key;
    std::string value;

    //first line: METHOD + PATH
    std::size_t i = line[0].find_first_of(' ');
    if (i == std::string::npos){
        _ret = 400;
        std::cerr << "Request Syntax Error: no space after method" << std::endl;
    }
    if (_ret != 400)
        setPath(req, i);
    _method.assign(line[0], 0, i);
    for (size_t j = 0; j < _methods.size(); j++)
    {
        if (_methods[j] == _method)
            break;
        if (j == _methods.size() - 1){
            std::cerr << "Request Error: Invalid Method[" << _method << "]" << std::endl;
            _ret = 400;
        }
    }
    
    //headers
    i = 1;
    for (; i < line.size() ; i++){
        //"The request/status line and headers must all end with <CR><LF>
        //(that is, a carriage return followed by a line feed)." 
        if (_ret == 400 || line[i] == "\r" || line[i] == "")
            break;
        key = line[i].substr(0, line[i].find_first_of(':'));
        value = line[i].substr(line[i].find_first_of(':') + 2);
        if (_headers.count(key)) //if header is compatible
            _headers[key] = value;
    }
    //body
    setBody(i, line);
}

Request::~Request(){}

std::vector<std::string>    Request::reqSplit(std::string req){
    std::vector<std::string>    line;
    std::size_t                 pos = 0;

    while ((pos = req.find("\r\n")) != std::string::npos){
        line.push_back(req.substr(0, pos));
        req.erase(0, pos + 2);
    }
    line.push_back(req);
    return (line);
}

void    Request::initHeaders(){
    _headers.clear();

    _headers["Accept-Charsets"] = "";
	_headers["Accept-Language"] = "";
	_headers["Allow"] = "";
	_headers["Auth-Scheme"] = "";
	_headers["Authorization"] = "";
	_headers["Content-Language"] = "";
	_headers["Content-Length"] = "";
	_headers["Content-Location"] = "";
	_headers["Content-Type"] = "";
	_headers["Date"] = "";
	_headers["Host"] = "";
	_headers["Last-Modified"] = "";
	_headers["Location"] = "";
	_headers["Referer"] = "";
	_headers["Retry-After"] = "";
	_headers["Server"] = "";
	_headers["Transfer-Encoding"] = "";
	_headers["User-Agent"] = "";
	_headers["Www-Authenticate"] = "";
	_headers["Connection"] = "Keep-Alive";
}

void    Request::initMethods(){
    _methods.push_back("GET");
    _methods.push_back("POST");
    _methods.push_back("DELETE");
}

void    Request::setPath(std::string req, std::size_t pos){
    std::size_t i = req.find_first_not_of(' ', pos);
    std::size_t j = req.find_first_of(' ', i);
    
    if (i == std::string::npos){
        _ret = 400;
        std::cerr << "Request Syntax Error: missing PATH or HTTP version" << std::endl;
        return ;
    }
    if (j == std::string::npos){
        _ret = 400;
        std::cerr << "Request Syntax Error: Missing HTTP version" << std::endl;
        return ;
    }
    _path.assign(req, i, j - i);
    j = req.find_first_not_of(' ', j);
    if (req[j] == 'H' && req[j + 1] == 'T' && req[j + 2] == 'T' && req[j + 3] == 'P' && req[j + 4] == '/')
        _version.assign(req, j + 5, 3);
    if (_version != "1.0" && _version != "1.1"){
        _ret = 400;
        std::cerr << "Incompatible HTTP version: use 1.0 or 1.1" << std::endl;
        return ;
    }
}

void    Request::setBody(std::size_t i, std::vector<std::string> line){
    if (line[i] != "")
        return ;
    i++;
    while (i < line.size() && line[i] != ""){
        _body += (line[i]);
        if (i + 1 != line.size())
            _body += "\r\n";
        i++;
    }
}

std::string Request::getPath() const{
    return (_path);
}

std::map<std::string, std::string>  Request::getHeaders() const{
    return (_headers);
}

std::string Request::getMethod() const{
    return (_method);
}

std::string Request::getBody() const{
    return (_body);
}