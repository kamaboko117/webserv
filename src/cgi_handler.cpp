/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 19:42:19 by asaboure          #+#    #+#             */
/*   Updated: 2022/10/20 14:26:03 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <map>
#include "Request.hpp"
#include "utils.hpp"
#include "ft_itoa_string.hpp"
#include "multipartReq.hpp"
#define BUFFERSIZE 32

bool        g_pending = false;
bool        g_cgipending = false;
std::string g_folder = "./home";
std::string g_boundary;
std::string g_file;
std::string g_head;
std::string g_body;
std::map<std::string, std::string> g_m_env;

// https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
std::map<std::string, std::string> CGISetEnv(Request &req){
    std::map<std::string, std::string> ret;
    std::map<std::string, std::string> headers = req.getHeaders();
    
    ret["REDIRECT_STATUS"] = "200";

    ret["SERVER_SOFTWARE"] = "webserv/1.0";
      ret["SERVER_NAME"] = "";//missing ==> get from conf (t_location?)
    ret["GATEWAY_INTERFACE"] = "CGI/1.1" ;
    
    ret["SERVER_PROTOCOL"] = "HTTP/1.1";
      ret["SERVER_PORT"] = ""; //missing ==> get from conf
    ret["REQUEST_METHOD"] = req.getMethod();
    ret["PATH_INFO"] = req.getPath().substr(0, req.getPath().find('?'));
    if (ret["PATH_INFO"] == "/")
        ret["PATH_INFO"] = ""; //conf
      ret["PATH_TRANSLATED"] = "." + ret["PATH_INFO"]; //conf path + path info basically (i think)
      ret["SCRIPT_NAME"] = ""; //missing ==> conf
    if (req.getPath().find('?') != std::string::npos)
        ret["QUERY_STRING"] = req.getPath().substr(req.getPath().find('?') + 1, std::string::npos);
      ret["REMOTE_HOST"] = "";
      ret["REMOTE_ADDR"] = ""; //==>conf
    //ret["AUTH_TYPE"] = "";
    ret["CONTENT_TYPE"] = headers["Content-Type"];
    ret["CONTENT_LENGTH"] = headers["Content-Length"];
    
    
    return(ret);
}

std::string errorPage(int code){
    std::string ret = "HTTP/1.1 ";
    std::string body = "error: ";

    ft_itoa_string(code, ret);
    ft_itoa_string(code, body);
    ret += "\r\nContent-Type: text/plain\r\nContent-Length: ";
    ft_itoa_string(body.length(), ret);
    ret += "\r\n\r\n";
    ft_itoa_string(code, body);
    ret += body;

    return (ret);
}

std::string executeCGI(std::map<std::string, std::string> m_env, std::string body){
    char        *args[3];
    int         fd[2];
    pid_t       cgiPID;
    FILE	    *fIn = tmpfile();
    long	    fdIn = fileno(fIn);
    std::string cgiRet;
    char        buf[BUFFERSIZE];
    
    args[0] = (char *)"/usr/bin/php-cgi";
    args[1] = ft_strdupcpp(m_env["PATH_TRANSLATED"].c_str());
    
    char **env = new char*[m_env.size() + 1];
    std::size_t i = 0;
    for (std::map<std::string, std::string>::iterator it = m_env.begin(); it != m_env.end(); it++){
        std::string var = it->first + "=" + it->second;
        env[i] = ft_strdupcpp(var.c_str());
        i++;
    }
    env[i] = NULL;
    if (body.size()){
        write(fdIn, body.c_str(), body.size());
	    lseek(fdIn, 0, SEEK_SET);
        g_body = "";
    }
    
    args[2] = NULL;
    pipe(fd);
    if ((cgiPID = fork()) == 0)
    {
        char * const * nll = NULL;
        close(STDOUT_FILENO);
        close(fd[0]);
        dup(fd[1]);
        dup2(fdIn, STDIN_FILENO);
        chdir(m_env["PATH_TRANSLATED"].substr(m_env["PATH_TRANSLATED"].find_last_of("/"), 0).c_str());
        execve(args[0], nll, env);
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
    fclose(fIn);
    close(fdIn);
    std::string retBody = cgiRet.substr(cgiRet.find("\r\n\r\n") + 2, std::string::npos);
    std::string retHeader = cgiRet.substr(0, cgiRet.find("\r\n\r\n"));
    // std::cout << "retHeader:" << std::endl << retHeader << std::endl;
    std::string ret = "HTTP/1.1 200 OK\nContent-Length: ";
    ft_itoa_string(retBody.size(), ret);
    
    std::size_t pos = retHeader.find("Status:");
    if (pos != std::string::npos){
        return (errorPage(ft_stoi(retHeader.substr(pos + 8, std::string::npos))));
    }
    ret += "\r\n" + retHeader;
    ret += "\r\n\r\n" + retBody;
    return (ret);
}

std::string continueCGIUpload(std::string strReq){
    std::string     ret = "HTTP/1.1 ";
    std::size_t     pos = 0;

    std::string     body = strReq;
    if (body.find("\r\n", body.size() - 2) == body.size() - 2)
        body.erase(body.size() -2, 10);
    g_body += body;
    pos = strReq.find(g_boundary + "--");
    if (pos == std::string::npos){
        ret += "100 Continue\r\n";
        return ("");
    } else {
        g_cgipending = false;
        return (executeCGI(g_m_env, g_body));
    }

    return (g_body);
}

std::string cgiHandler(std::map<std::string, std::string> m_env, Request &req, std::string strReq)//, t_location location)
{
    // for (std::map<std::string, std::string>::iterator it = m_env.begin(); it != m_env.end(); it++)
    //     std::cout << "key: " << it->first << " | value: " << it->second << std::endl;

    std::cout << "ENV: " << m_env["CONTENT_TYPE"] << std::endl;
    if (m_env["CONTENT_TYPE"].substr(0, m_env["CONTENT_TYPE"].find(';')) == "multipart/form-data"){
        g_cgipending = true;
        g_head = strReq;
        std::string contentType = req.getHeaders()["Content-Type"];
        g_boundary = "--" + contentType.substr(contentType.find("; boundary=") + 11, std::string::npos);
        g_m_env = m_env;
        if (req.getBody() != "")
            return (continueCGIUpload(strReq.substr(strReq.find("\r\n\r\n") + 4, std::string::npos)));
        return ("");
    }
    
    return (executeCGI(m_env, req.getBody()));
}

std::string transferFile(std::string type, std::string file){
    std::string         ret = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\nContent-Length: ";
    std::ifstream       f(file.c_str());
    std::stringstream   ss;
    
    ss << f.rdbuf();
    ft_itoa_string(ss.str().length(), ret);
    ret += "\r\n\r\n";
    ret += ss.str();

    // std::cout << "ret:\n" << ret << std::endl;
    return (ret);
}

// std::string uploadFile(std::map<std::string, std::string> m_env, Request &req){
//     //g_file = m_env["PATH_TRANSLATED"];
//     // g_length = ft_stol(req.getHeaders()["Content-Length"]);
//    
//     std::ofstream   outfile(m_env["PATH_TRANSLATED"].c_str());
//     std::string     ret;
//    
//     if (!outfile)
//         return (errorPage(500));
//     outfile << req.getBody() << std::endl;
//     outfile.close();
//     if (outfile.tellp() < g_length){
//         ret = "HTTP/1.1 100\r\n";
//         g_pending = true;
//     }
//     else{
//         ret = "HTTP/1.1 201\r\n";
//         g_pending = false;
//     }
//     ret += "Content-Length: 0\r\nLocation: ";
//     ret += m_env["PATH_TRANSLATED"] + "\r\n\r\n";
//     std::cout << "inpending: " << g_pending << std::endl; 
//     return (ret);
// }

std::string continueUpload(std::string strReq){
    std::string     ret = "HTTP/1.1 ";
    std::size_t     pos = 0;

    if (strReq.find(g_boundary) == 0){
        MultipartReq    req(strReq.substr(strReq.find('\n') + 1, std::string::npos));
        g_file = g_folder + "/" + req.getFilename();
        if (existsFile(g_file))
        {
            std::size_t i = 1;
            std::string name = req.getFilename().substr(0, req.getFilename().find_last_of('.'));
            std::string ext = "";
            if (req.getFilename().find_last_of('.') != std::string::npos)
                ext = req.getFilename().substr(req.getFilename().find_last_of('.'), std::string::npos);
            while (existsFile(g_folder + "/" + name + " (" + ft_itoa_string(i) + ")" + ext))
                i++;
            g_file = g_folder + "/" + name  + " (" + ft_itoa_string(i) + ")" + ext;
            std::cout << "name: " << name << " ext: " << ext << std::endl;
        }
        pos = strReq.find("\r\n\r\n") + 4;
    }
    std::string     body = strReq.substr(pos, strReq.find(g_boundary, pos) - pos);
    if (body.find("\r\n", body.size() - 2) == body.size() - 2)
        body.erase(body.size() -2, 10);
    std::ofstream   outfile(g_file.c_str(), std::ios_base::app | std::ios_base::binary);
	std::cout << "g_file = " << g_file << std::endl;
	if (!outfile) {
		return (errorPage(500));
	}
    outfile << body;
    pos = strReq.find(g_boundary + "--");
    if (pos == std::string::npos){
        ret += "100 Continue\r\n";
        return ("");
    } else {
        ret += "201\r\nContent-Length: 0\r\nContent-Type: text/plain\r\nLocation: ";
        ret +=  g_file + "\r\n\r\n";
        g_pending = false;
    }

    return (ret);
}

// std::string post(std::map<std::string, std::string> m_env, Request &req){
//     //if (!existsFile(m_env["PATH_TRANSLATED"])){
//         return (uploadFile(m_env, req));
//     //}
//
//     std::string         ret = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n";
//     std::ifstream       f(m_env["PATH_TRANSLATED"].c_str())
//
//     // std::cout << "ret:\n" << ret << std::endl;
//     return (ret);
// }

std::string multipartHandler(Request &req, std::string strReq){
    std::string     ret = "HTTP/1.1 ";

    g_pending = true;
    std::string contentType = req.getHeaders()["Content-Type"];
    g_boundary = "--" + contentType.substr(contentType.find("; boundary=") + 11, std::string::npos);
    //g_folder = m_env["PATH_TRANSLATED"]; // check if this is a directory
    if (req.getBody() != "")
        return (continueUpload(strReq.substr(strReq.find("\r\n\r\n") + 4, std::string::npos)));
    ret += "100 Continue\r\n";

    //return (ret);
    return ("");
}

std::string deleteHandler(std::map<std::string, std::string> m_env){
    std::string ret = "HTTP/1.1 ";

    if (remove(m_env["PATH_TRANSLATED"].c_str()))
        return (errorPage(403));
    ret += " 200\r\nContent-Length: ";
    std::string body = m_env["PATH_TRANSLATED"] + " succesfully deleted";
    ft_itoa_string(body.size(), ret);
    ret += "\r\nContent-Type: text/plain\r\n\r\n" + body + "\r\n";
    return (ret);
}

std::string autoindex(std::string path, std::map<std::string, std::string> m_env){
    m_env["QUERY_STRING"] = "PATH=" + path;
    m_env["PATH_TRANSLATED"] = "./autoindex.php";
    return (executeCGI(m_env, ""));
}

std::string requestHandler(std::string strReq){  
    if (g_pending)
        return (continueUpload(strReq));
    if (g_cgipending)
        return (continueCGIUpload(strReq));

    std::map<std::string, std::string>  m_env;
    Request                             req(strReq);
    std::string                         type;
    
    m_env = CGISetEnv(req);

    std::string extension = "";
    if (m_env["PATH_INFO"].find_last_of('.') != std::string::npos)
        extension = m_env["PATH_INFO"].substr(m_env["PATH_INFO"].find_last_of('.'), std::string::npos);
    
    if (extension == ".php" || extension == ".html")
        return (cgiHandler(m_env, req, strReq));

    if (!existsFile(m_env["PATH_TRANSLATED"]) && m_env["REQUEST_METHOD"] != "POST")
        return errorPage(404);
    if (!canRead(m_env["PATH_TRANSLATED"]) && m_env["REQUEST_METHOD"] == "GET")
        return errorPage(403);
    if (isDirectory(m_env["PATH_TRANSLATED"]) && m_env["REQUEST_METHOD"] != "POST") //&& autoindex is on
        return (autoindex(m_env["PATH_TRANSLATED"], m_env));
    if (m_env["REQUEST_METHOD"] == "POST"){
        if (m_env["CONTENT_TYPE"].substr(0, m_env["CONTENT_TYPE"].find(';')) == "multipart/form-data")
            return (multipartHandler(req, strReq));
        // return (post(m_env, req));
    }
    else if (m_env["REQUEST_METHOD"] == "DELETE"){
        return (deleteHandler(m_env));
    }

    else if (extension == ".ico")
        type = "images/x-icon";
    else if (extension == ".mp4")
        type = "video/mp4";
    else
        type = "text/plain";
    
    return (transferFile(type, m_env["PATH_TRANSLATED"]));
}
