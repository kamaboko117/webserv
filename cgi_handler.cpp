/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_handler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaboure <asaboure@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/05 19:42:19 by asaboure          #+#    #+#             */
/*   Updated: 2022/09/06 15:12:24 by asaboure         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
#define BUFFERSIZE 32

// https://forhjy.medium.com/42-webserv-cgi-programming-66d63c3b22db
std::string cgiHandler(char **args, char **env)
{
    int         fd[2];
    pid_t       cgiPID;
    std::string retBody;
    char        buf[BUFFERSIZE];

    pipe(fd);
    if ((cgiPID = fork()) == 0)
    {
        close(fd[1]);
        dup2(fd[0], 0);
        execve(args[0], args, env);
    }
    else
    {
        wait(NULL);
        int retRead = 1;
        while (retRead)
        {
            retRead = read(fd[1], buf, BUFFERSIZE);
            retBody += buf;
        }
    }
    if (cgiPID == 0)
        exit(0);
    return (retBody);
}

int main(){
    char *args[2];
    args[0] = (char *)"/usr/bin/php-cgi7.4";
    args[1] = (char *)"test.php";

    std::string body = cgiHandler(args, NULL);
    std::cout << "pouet" << std::endl;
    std::cout << body << std::endl;
}