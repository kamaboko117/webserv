/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 19:42:19 by asaboure          #+#    #+#             */
/*   Updated: 2022/09/22 14:59:37 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
#include <string.h>
#include <map>
#include "Request.hpp"
#include "ft_itoa_string.hpp"
#define BUFFERSIZE 32

// https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
std::map<std::string, std::string> CGISetEnv(Request &req){
    std::map<std::string, std::string> ret;
    std::map<std::string, std::string> headers = req.getHeaders();
    
    ret["REDIRECT_STATUS"] = "200";

    ret["SERVER_SOFTWARE"] = "webserv/1.0";
      ret["SERVER_NAME"] = //missing ==> get from conf (t_location?)
    ret["GATEWAY_INTERFACE"] = "CGI/1.1" ;
    
    ret["SERVER_PROTOCOL"] = "HTTP/1.1";
      ret["SERVER_PORT"] = ""; //missing ==> get from conf
    ret["REQUEST_METHOD"] = req.getMethod();
    ret["PATH_INFO"] = req.getPath().substr(0, req.getPath().find('?'));
    if (ret["PATH_INFO"] == "/")
        ret["PATH_INFO"] = "/home/form.html"; //conf
      ret["PATH_TRANSLATED"] = "/mnt/nfs/homes/asaboure/42/webserv" + ret["PATH_INFO"]; //conf path + path info basically (i think)
      ret["SCRIPT_NAME"] = ""; //missing ==> conf
    ret["QUERY_STRING"] = req.getPath().substr(req.getPath().find('?') + 1, std::string::npos);
      ret["REMOTE_HOST"] = "";
      ret["REMOTE_ADDR"] = ""; //==>conf
    //ret["AUTH_TYPE"] = "";
    ret["CONTENT_TYPE"] = headers["CONTENT_TYPE"];
    
    
    return(ret);
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

std::string cgiHandler(std::string strReq)//, t_location location)
{
    std::cout << std::endl << "*****************************************" <<std::endl;
    std::cout << "ENTERED CGIHANDLER" << std::endl;

    Request                             req(strReq);
    int                                 fd[2];
    pid_t                               cgiPID;
    std::string                         cgiRet;
    char                                buf[BUFFERSIZE];
    char                                *args[3];
    std::map<std::string, std::string>  m_env;
    char                                **env;

    std::cout << "/*/*/*/*" << std::endl;
    m_env = CGISetEnv(req);
    args[0] = (char *)"/usr/bin/php-cgi7.4";
    args[1] = ft_strdupcpp(m_env["PATH_TRANSLATED"].c_str());
    args[2] = NULL;
    std::cout << "/*/*/*/*" << std::endl;
    for (std::map<std::string, std::string>::iterator it = m_env.begin(); it != m_env.end(); it++)
        std::cout << "key: " << it->first << " | value: " << it->second << std::endl;

    env = new char*[m_env.size() + 1];
    std::size_t i = 0;
    for (std::map<std::string, std::string>::iterator it = m_env.begin(); it != m_env.end(); it++){
        std::string var = it->first + "=" + it->second;
        env[i] = ft_strdupcpp(var.c_str());
        i++;
    }
    env[i] = NULL;
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
            cgiRet += buf;
        }
    }

    if (cgiPID == 0)
        return ("");
    for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
    delete[] args[1];
    std::cout << std::endl << "*************************" << std::endl;
    std::cout << "CGI RETURN:" << std::endl << cgiRet << std::endl;

    std::string retBody = cgiRet.substr(cgiRet.find("\r\n\r\n") + 2, std::string::npos);
    std::string retHeader = cgiRet.substr(0, cgiRet.find("\r\n\r\n"));
    std::cout << "retHeader:" << std::endl << retHeader << std::endl;
    std::string ret = "HTTP/1.1 200 OK\nContent-Length: ";
    ft_itoa_string(retBody.size(), ret);
    ret += "\n" + retHeader;
    ret += "\n\n" + retBody;
    return (ret);
}

// int main(){
//     // char *args[2];
//     // args[0] = (char *)"/usr/bin/php-cgi7.4";
//     // args[1] = (char *)"hello.php";

//     // std::string body = cgiHandler(args, NULL);
//     // std::cout << body << std::endl;
// }