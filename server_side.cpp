#include <stdio.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdlib>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 	8080
#define IPv6	AF_INET6
#define IPv4	AF_INET

// c_str() is string to ( CONST char * )
// String to char *
void	ft_string_to_char(std::string s, char *c) {
	c = (char *)calloc(s.size() + 1, sizeof(char));
	strcpy(c, s.c_str());
}

int main(int argc, char const *argv[])
{
	int server_fd, new_socket; long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Only this line has been changed. Everything is same.

	https://en.wikipedia.org/wiki/List_of_HTTP_status_codes

	//char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	// Creating socket file descriptor
	if ((server_fd = socket(IPv4, SOCK_STREAM, getprotobyname("tcp")->p_proto)) == -1)
	{
		perror("In socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = IPv4;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	memset(address.sin_zero, '\0', sizeof address.sin_zero);


	// Indentify a socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)))
	{
		perror("In bind");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	// Wait for incomming request/connection
	if (listen(server_fd, 1))
	{
		perror("In listen");
		exit(EXIT_FAILURE);
	}

	int fd_test_html;
	std::string toto;
	std::string	file_content;
	// Responde from the server (alowd / not alowd, Not exist, etc...)
	toto.append("HTTP/1.1 200 OK\n");

	// The type of the file requested
	toto.append("Content-type: text/html\n\n");

	// Open and read the content store it in a string
	fd_test_html = open("home/test.html", O_RDWR);
	char buffer;
	while (read(fd_test_html, &buffer, 1))
		file_content.append(&buffer, 1);
	close(fd_test_html);
	fd_test_html = open("home/test.html", O_RDWR);
	if (fd_test_html < 0)
	{
		perror("open");
		exit(-1);
	}
	toto.append("Content-Length: ");
	char *titi = itoa(file_content.size(), 10);
	toto.append(std::string(titi));
	toto.append("\n\n");

	while(1)
	{
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) == -1)
		{
			perror("In accept");
			exit(EXIT_FAILURE);
		}

		char buffer[30000] = {0};
		valread = read( new_socket , buffer, 30000);
		printf("%s\n",buffer );
		write(new_socket , toto.c_str() , strlen(toto.c_str()));
		printf("------------------toto message sent-------------------");
		// Close the socket / the filedescriptor
		close(new_socket);
	}
	return 0;
}
