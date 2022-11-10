/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 19:42:19 by asaboure          #+#    #+#             */
/*   Updated: 2022/11/10 17:07:51 by asaboure         ###   ########.fr       */
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
#include <algorithm>
#include <dirent.h>
#define BUFFERSIZE 32

bool        g_pending = false;
bool        g_cgipending = false;
std::string g_folder = "./home";
std::string g_boundary;
std::string g_file;
std::string g_head;
std::string g_body;
std::map<std::string, std::string> g_m_env;

std::string requestHandler(std::string strReq, cfg::Server server);

// https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
std::map<std::string, std::string> CGISetEnv(Request &req, cfg::Server server, cfg::t_location location){
    std::map<std::string, std::string> ret;
    std::map<std::string, std::string> headers = req.getHeaders();
    std::stringstream   ss;
    std::string         extension = "";
    ret["REDIRECT_STATUS"] = "200";

    ret["SERVER_SOFTWARE"] = "webserv/1.0";
    ret["SERVER_NAME"] = server._server_name[0];
    ret["GATEWAY_INTERFACE"] = "CGI/1.1" ;
    
    ret["SERVER_PROTOCOL"] = "HTTP/1.1";
    ss << server._listen;
    ret["SERVER_PORT"] = ss.str();
    ret["REQUEST_METHOD"] = req.getMethod();
    ret["PATH_INFO"] = req.getPath().substr(0, req.getPath().find('?'));
    if (ret["PATH_INFO"] == "/")
        ret["PATH_INFO"] = "";
    ret["PATH_TRANSLATED"] = "." + ret["PATH_INFO"].replace(0, location._location.size(), location._root); //conf path + path info basically (i think)
    if (ret["PATH_TRANSLATED"].find_last_of('.') != std::string::npos)
        extension = ret["PATH_TRANSLATED"].substr(ret["PATH_TRANSLATED"].find_last_of('.'), std::string::npos);
    ret["SCRIPT_NAME"] = location._cgi_pass[extension];
    if (req.getPath().find('?') != std::string::npos)
        ret["QUERY_STRING"] = req.getPath().substr(req.getPath().find('?') + 1, std::string::npos);
      ret["REMOTE_HOST"] = "";
      ret["REMOTE_ADDR"] = ""; //==>conf
    //ret["AUTH_TYPE"] = "";
    ret["CONTENT_TYPE"] = headers["Content-Type"];
    ret["CONTENT_LENGTH"] = headers["Content-Length"];

    return(ret);
}

std::string errorPage(int code, cfg::Server server){
    std::string ret = "HTTP/1.1 ";
    ft_itoa_string(code, ret);
    std::string body;
    if (server._error_page.count(code)){
        // std::ifstream       f(server._error_page[code].c_str());
        // std::stringstream   ss;

        // ss << f.rdbuf();
        // body = ss.str();
        return (requestHandler("GET /" + server._error_page[code] + " HTTP/1.1\r\n", server));
    } else {
        body = "error: ";
        ft_itoa_string(code, body);
    }
    body += "\r\n";
    ret += "\r\nContent-Type: text/plain\r\nContent-Length: ";
    ft_itoa_string(body.length(), ret);
    ret += "\r\n\r\n";
    ret += body;

    return (ret);
}

std::string executeCGI(std::map<std::string, std::string> m_env, std::string body, cfg::Server server){
    char        *args[3];
    int         fd[2];
    pid_t       cgiPID;
    FILE	    *fIn = tmpfile();
    long	    fdIn = fileno(fIn);
    std::string cgiRet;
    char        buf[BUFFERSIZE];
    
    args[0] = ft_strdupcpp(m_env["SCRIPT_NAME"].c_str());
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
        chdir(m_env["PATH_TRANSLATED"].substr(m_env["PATH_TRANSLATED"].find_last_of("/"), std::string::npos).c_str());
        execve(args[0], nll, env);
        std::cerr << "Execve crashed." << std::endl;
        write(fd[1], "Status: 500\r\n\r\n", 15);
        exit (0);
    }
    else
    {
        waitpid(cgiPID, NULL, 0);
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
    delete[] args[0];
    delete[] args[1];
    fclose(fIn);
    close(fdIn);
    std::string retBody = cgiRet.substr(cgiRet.find("\r\n\r\n") + 2, std::string::npos);
    std::string retHeader = cgiRet.substr(0, cgiRet.find("\r\n\r\n"));
    std::string ret = "HTTP/1.1 200 OK\nContent-Length: ";
    ft_itoa_string(retBody.size(), ret);
    
    std::size_t pos = retHeader.find("Status:");
    if (pos != std::string::npos){
        return (errorPage(ft_stoi(retHeader.substr(pos + 8, std::string::npos)), server));
    }
    ret += "\r\n" + retHeader;
    ret += "\r\n\r\n" + retBody;
    return (ret);
}

std::string continueCGIUpload(std::string strReq, cfg::Server server){
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
        return (executeCGI(g_m_env, g_body, server));
    }

    return (g_body);
}

std::string cgiHandler(std::map<std::string, std::string> m_env, Request &req, std::string strReq, cfg::Server server)
{
    if (m_env["CONTENT_TYPE"].substr(0, m_env["CONTENT_TYPE"].find(';')) == "multipart/form-data"){
        g_cgipending = true;
        g_head = strReq;
        std::string contentType = req.getHeaders()["Content-Type"];
        g_boundary = "--" + contentType.substr(contentType.find("; boundary=") + 11, std::string::npos);
        g_m_env = m_env;
        if (req.getBody() != "")
            return (continueCGIUpload(strReq.substr(strReq.find("\r\n\r\n") + 4, std::string::npos), server));
        return ("");
    }
    
    return (executeCGI(m_env, req.getBody(), server));
}

std::string transferFile(std::string type, std::string file){
    std::string         ret = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\nContent-Length: ";
    std::ifstream       f(file.c_str());
    std::stringstream   ss;
    
    ss << f.rdbuf();
    ft_itoa_string(ss.str().length(), ret);
    ret += "\r\n\r\n";
    ret += ss.str();

    return (ret);
}

std::string continueUpload(std::string strReq, cfg::Server server){
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
        }
        pos = strReq.find("\r\n\r\n") + 4;
    }
    std::string     body = strReq.substr(pos, strReq.find(g_boundary, pos) - pos);
    if (body.find("\r\n", body.size() - 2) == body.size() - 2)
        body.erase(body.size() -2, 10);
    std::ofstream   outfile(g_file.c_str(), std::ios_base::app | std::ios_base::binary);
	if (!outfile) {
        std::cout << "g_file: " << g_file << std::endl;
		return (errorPage(500, server));
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

std::string multipartHandler(Request &req, std::string strReq, cfg::t_location location, cfg::Server server){
    std::string     ret = "HTTP/1.1 ";
std::cout << "**********multipart******" << std::endl;
    g_pending = true;
    std::string contentType = req.getHeaders()["Content-Type"];
    g_boundary = "--" + contentType.substr(contentType.find("; boundary=") + 11, std::string::npos);
    g_folder = location._root + "/" + location._upload_store; // check if this is a directory
    if (req.getBody() != "")
        return (continueUpload(strReq.substr(strReq.find("\r\n\r\n") + 4, std::string::npos), server));
    ret += "100 Continue\r\n";

    //return (ret);
    return ("");
}

std::string deleteHandler(std::map<std::string, std::string> m_env, cfg::Server server){
    std::string ret = "HTTP/1.1 ";

    if (remove(m_env["PATH_TRANSLATED"].c_str()))
        return (errorPage(403, server));
    ret += " 200\r\nContent-Length: ";
    std::string body = m_env["PATH_TRANSLATED"] + " succesfully deleted";
    ft_itoa_string(body.size(), ret);
    ret += "\r\nContent-Type: text/plain\r\n\r\n" + body + "\r\n";
    return (ret);
}

std::string autoindex(char const *dir_path)
{
    DIR                    *dh;
    struct dirent        *contents;
    std::string             head = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
    std::string            autoindex;

    dh = opendir(dir_path);
    if (!dh)
        return std::string("<body>dir_path not found !</body>");
    autoindex.append("<body><ul>");
    autoindex.append("<li><a href=\"/.\">.</li>");
    autoindex.append("<li><a href=\"/..\">..</li>");
    while ((contents = readdir(dh)) != NULL)
    {
        if (std::string(contents->d_name) != "." && std::string(contents->d_name) != "..")
        {
            autoindex.append("<li><a href=\"/");
            autoindex.append(dir_path);
            if (*autoindex.rbegin() != '/')
                autoindex.append("/");
            autoindex.append(contents->d_name);
            autoindex.append("\">");
            autoindex.append(contents->d_name);
            autoindex.append("</li>");
        }
    }
    autoindex.append("</body></ul>");
    closedir(dh);
    ft_itoa_string(autoindex.length(), head);
    return (head + "\r\n\r\n" + autoindex);
}

std::string getValidIndex(std::vector<std::string> indexes){
    for (size_t i = 0; i < indexes.size(); i++)
    {
        if (existsFile(indexes[i]) && canRead(indexes[i]))
            return indexes[i];
    }
    return ("");
}

std::string uploadFile(std::map<std::string, std::string> m_env, Request &req, cfg::Server server, cfg::t_location location){
    g_file = location._upload_store;
    long length = ft_stol(req.getHeaders()["Content-Length"]);
   
    std::ofstream   outfile(m_env["PATH_TRANSLATED"].c_str());
    std::string     ret;
   
    if (!outfile)
        return (errorPage(500, server));
    outfile << req.getBody() << std::endl;
    outfile.close();
    if (outfile.tellp() < length){
        ret = "HTTP/1.1 100\r\n";
        g_pending = true;
    }
    else{
        ret = "HTTP/1.1 201\r\n";
        g_pending = false;
    }
    ret += "Content-Length: 0\r\nLocation: ";
    ret += m_env["PATH_TRANSLATED"] + "\r\n\r\n";
    return (ret);
}

std::string redirect(cfg::t_location location){
    std::string ret = "HTTP/1.1 ";
    ft_itoa_string(location._return.first, ret);
    ret += "\r\nLocation: " + location._return.second + "\r\n\r\n";
    return (ret);
}

std::string requestHandler(std::string strReq, cfg::Server server){  
    if (g_pending)
        return (continueUpload(strReq, server));
    if (g_cgipending)
        return (continueCGIUpload(strReq, server));
    
    std::map<std::string, std::string>  m_env;
    Request                             req(strReq);
    std::string                         type;
    
    //request class sets its ret variable to 200 if request is correct
    if(req.getRet() != 200)
        return (errorPage(req.getRet(), server));
    std::vector<cfg::t_location>::iterator it = closestMatchingLocation(server, req.getPath());
    if (it == server._locations.end())
        return (errorPage(404, server));

    //check redirection
    if (it->_return.first)
        return (redirect(*it));

    //set root if exists
    if (it->_root == "")
        it->_root = "./" + it->_location;
    
    //check if method is allowed
    if (std::find(it->_allow.begin(), it->_allow.end(), req.getMethod()) == it->_allow.end())
        return (errorPage(405, server));

    std::string extension = "";
    //set environment for cgi
    m_env = CGISetEnv(req, server, *it);
    
    if (m_env["PATH_TRANSLATED"].find_last_of('.') != std::string::npos)
        extension = m_env["PATH_TRANSLATED"].substr(m_env["PATH_TRANSLATED"].find_last_of('.'), std::string::npos);
    if ((extension == ".php") && it->_cgi_pass.find(".php") != it->_cgi_pass.end())
        return (cgiHandler(m_env, req, strReq, server));

    //check if file exists
    if (!existsFile(m_env["PATH_TRANSLATED"]) && m_env["REQUEST_METHOD"] != "POST" && m_env["PATH_INFO"] != it->_root){
        std::cout << "\n*****file " << m_env["PATH_TRANSLATED"] << " does not exist" << std::endl;
        return errorPage(404, server);
    }
    //check if file is readable
    if (!canRead(m_env["PATH_TRANSLATED"]) && m_env["REQUEST_METHOD"] == "GET" && m_env["PATH_INFO"] != it->_root)
        return errorPage(403, server);
    
    //check if file is a directory => index or autoindex
    if ((isDirectory(m_env["PATH_TRANSLATED"]) || m_env["PATH_INFO"] == it->_root) && m_env["REQUEST_METHOD"] != "POST"){
        if (it->_autoindex)
            return (autoindex(m_env["PATH_TRANSLATED"].c_str()));
        if (!it->_index.size()){
            std::cout << "\n*****no index" << std::endl;
            return (errorPage(404, server));
        }
        m_env["PATH_TRANSLATED"] = getValidIndex(it->_index);
        if (m_env["PATH_TRANSLATED"] == ""){
            std::cout << "\n*****indexes found but none valid" << std::endl;
            return (errorPage(404, server));
        }
        extension = m_env["PATH_TRANSLATED"].substr(m_env["PATH_TRANSLATED"].find_last_of('.'), std::string::npos);
        if ((extension == ".php") && it->_cgi_pass.find(".php") != it->_cgi_pass.end())
            return (cgiHandler(m_env, req, strReq, server));
    }

    //handle posts
    if (m_env["REQUEST_METHOD"] == "POST"){
        if (server._client_max_body_size && (std::size_t)ft_stoi(m_env["CONTENT_LENGTH"]) > server._client_max_body_size)
            return (errorPage(400, server));
        if (m_env["CONTENT_TYPE"].substr(0, m_env["CONTENT_TYPE"].find(';')) == "multipart/form-data")
            return (multipartHandler(req, strReq, *it, server));
        return (uploadFile(m_env, req, server, *it));
    }

    //handle deletes
    else if (m_env["REQUEST_METHOD"] == "DELETE"){
        return (deleteHandler(m_env, server));
    }
    //from here method can only be GET. 
    else if (extension == ".ico")
        type = "images/x-icon";
    else if (extension == ".mp4")
        type = "video/mp4";
    else if (extension == ".html")
        type = "text/html";
    else
        type = "text/plain";
    return (transferFile(type, m_env["PATH_TRANSLATED"]));
}
