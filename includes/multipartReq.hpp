/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multipartReq.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/05 18:28:35 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/06 13:13:38 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPARTREQ_HPP
# define MULTIPARTREQ_HPP
# include <string>
# include <vector>
# include <map>

class MultipartReq
{
private:
    /*** GARBAGE FOR CANONICAL FORM NORM***/
    MultipartReq();
    MultipartReq(const MultipartReq &src);
    MultipartReq &operator=(const MultipartReq &rhs);
    
    /*** private functions ***/
    void                        initHeaders();
    std::vector<std::string>    reqSplit(std::string);

    /*** ETC ***/
    std::map<std::string, std::string>    _headers;

    std::string _contentDisposition;
    std::string _name;
    std::string _filename;
    std::string _contentType;

public:
    MultipartReq(const std::string req);
    virtual ~MultipartReq();

    std::string getContentDisposition() const;
    std::string getName() const;
    std::string getFilename() const;
    std::string getContentType() const;
};

#endif