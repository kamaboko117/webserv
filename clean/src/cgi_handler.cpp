/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 19:42:19 by asaboure          #+#    #+#             */
/*   Updated: 2022/09/21 14:13:53 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#include <map>
#include "Request.hpp"
#define BUFFERSIZE 32

// https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
std::map<std::string, std::string> CGISetEnv(Request &req){
    std::map<std::string, std::string> ret;
    std::map<std::string, std::string> headers = req.getHeaders();
    
    ret["SERVER_SOFTWARE"] = "webserv/1.0";
    ret["SERVER_NAME"] = //missing ==> get from conf (t_location?)
    ret["GATEWAY_INTERFACE"] = "CGI/1.1" ;
    
    ret["SERVER_PROTOCOL"] = "HTTP/1.1";
    ret["SERVER_PORT"] = ""; //missing ==> get from conf
    ret["REQUEST_METHOD"] = req.getMethod();
    ret["PATH_INFO"] = req.getPath();
    ret["PATH_TRANSLATED"] = req.getPath(); //??????
    ret["SCRIPT_NAME"] = ""; //missing ==> conf
    ret["QUERY_STRING"] = req.getPath().substr(req.getPath().find('?'), std::string::npos);
    
    return(ret);
}

std::string cgiHandler(std::string strReq)//, t_location location)
{
    std::cout << std::endl << "*****************************************" <<std::endl;
    std::cout << "ENTERED CGIHANDLER" << std::endl;

    Request                             req(strReq);
    int                                 fd[2];
    pid_t                               cgiPID;
    std::string                         retBody;
    char                                buf[BUFFERSIZE];
    char                                *args[2];
    std::map<std::string, std::string>  m_env;
    char                                **env;

    args[0] = (char *)"/usr/bin/php-cgi7.4";
    args[1] = strdup(req.getPath().substr(0, req.getPath().find('?')).c_str());
    m_env = CGISetEnv(req);

    for (std::map<std::string, std::string>::iterator it = m_env.begin(); it != m_env.end(); it++)
        std::cout << "key: " << it->first << " | value: " << it->second << std::endl;

    pipe(fd);
    if ((cgiPID = fork()) == 0)
    {
        close(STDOUT_FILENO);
        close(fd[0]);
        dup(fd[1]);
        execve(args[0], args, env);
    }
    else
    {
        wait(NULL);
        close(STDIN_FILENO);
        close(fd[1]);
        dup(fd[0]);
        lseek(fd[0], 0, SEEK_SET);
        int retRead = 1;
        while (retRead > 0)
        {
            bzero(buf, BUFFERSIZE);
            retRead = read(fd[0], buf, BUFFERSIZE - 1);
            retBody += buf;
        }
    }
    if (cgiPID == 0)
        return ("");
    return (retBody);
}

// int main(){
//     // char *args[2];
//     // args[0] = (char *)"/usr/bin/php-cgi7.4";
//     // args[1] = (char *)"hello.php";

//     // std::string body = cgiHandler(args, NULL);
//     // std::cout << body << std::endl;
// }