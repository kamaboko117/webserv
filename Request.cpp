/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/19 14:20:16 by asaboure          #+#    #+#             */
/*   Updated: 2022/09/19 15:52:52 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include <vector>

std::vector<std::string>    reqSplit(std::string req){
    std::vector<std::string>    split;
    std::size_t                 pos = 0;

    while ((pos = req.find('\n')) != std::string::npos){
        split.push_back(req.substr(0, pos));
        req.erase(0, pos + 1);
    }
    return (split);
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

Request::Request(const std::string req){
    initHeaders();
    std::vector<std::string> split = reqSplit(req);
    std::string key;
    std::string value;
    
    for (std::vector<std::string>::iterator it = split.begin(); it != split.end(); it++){

    }
}