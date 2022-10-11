/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multipartReq.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/05 18:33:42 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/06 15:16:26 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "multipartReq.hpp"

MultipartReq::MultipartReq(std::string strReq){
    initHeaders();
    std::vector<std::string> line = reqSplit(strReq);
    std::string key;
    std::string value;
    for (size_t i = 0; i < line.size(); i++)
    {
        if (line[i] == "\r" || line[i] == "")
            break;
        key = line[i].substr(0, line[i].find_first_of(':'));
        value = line[i].substr(line[i].find_first_of(':') + 2);
        if (_headers.count(key))
            _headers[key] = value;
    }
    _contentDisposition = _headers["Content-Disposition"];
    std::size_t pos = _contentDisposition.find(';');
    if (_contentDisposition.substr(0, pos) == "form-data"){
        pos = _contentDisposition.find("name=");
        _name = _contentDisposition.substr(pos + 5, _contentDisposition.find(';', pos));
        pos = _contentDisposition.find("filename=");
        _filename = _contentDisposition.substr(pos + 10, _contentDisposition.size() - (pos + 10 + 1));
    }
    _contentType = _headers["Content-Type"];
}

std::vector<std::string>    MultipartReq::reqSplit(std::string req){
    std::vector<std::string>    line;
    std::size_t                 pos = 0;

    while ((pos = req.find("\r\n")) != std::string::npos){
        line.push_back(req.substr(0, pos));
        req.erase(0, pos + 2);
    }
    line.push_back(req);
    return (line);
}

void    MultipartReq::initHeaders(){
    _headers.clear();

    _headers["Content-Disposition"] = "";
    _headers["Content-Type"] = "";
}

std::string MultipartReq::getContentDisposition() const{
    return (_contentDisposition);
}
std::string MultipartReq::getName() const{
    return (_name);
}
std::string MultipartReq::getFilename() const{
    return (_filename);
}
std::string MultipartReq::getContentType() const{
    return (_contentType);
}

MultipartReq::~MultipartReq(){}