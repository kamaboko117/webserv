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
#include <sys/wait.h>
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

int	pars(const char *str) {
	int i = 5;
	//printf("str[i] = %c\n", str[i]);
	if (str[i] == 'f')
		return (-1);
	if (str[i] == '1')
		return (1);
	if (str[i] == '2')
		return (2);
	if (str[i] == '3')
		return (3);
	return (0);
}

void	open_file(const char *file, int socket) {
	std::string file_content;

	/* Réponse du serveur au client */
	file_content.append("HTTP/1.1 200 OK\n");

	/* Type de document renvoyé au navigateur */
	file_content.append("Content-type: text/html\n");

	/* Cookie que le navigateur va stocker */

	/* Bonus Cookie (session)
	 * Idée d'exemple serai un simple bouton [conect] et/ou [disconect]
	 * (sur une page dédié)
	 * Cela mettra a jour les pages et diras en bas s'il est connecté (ou non)
	 */
	file_content.append("Set-Cookie: toto=titi\n\n");

	/* Longueur du fichier LUS par le client
	 * (Le contenu peut être plus grand que la taille donné)
	 * Cela est uniquement la taille que renvoie le CGI
	 * (sans compte l'en tête HTTP)
	 */
	//file_content.append("Content-Length: 1024\n\n");

	int fd = open(file, O_RDWR);
	char buffer;
	while (read(fd, &buffer, 1)) {
		file_content.append(&buffer, 1);
	}

	/* Depending of the background, different flag should be maid*/
	send(socket, file_content.c_str(), file_content.size(), MSG_CONFIRM);
}

void	send_page(int i, int socket) {
	switch (i) {
		case 1:
			open_file("home/file1.html", socket);
			break;
		case 2:
			open_file("home/file2.html", socket);
			break;
		case 0:
			open_file("home/default.html", socket);
			break;
		case 3:
			open_file("home/form.html", socket);
			break;
	}
}

int main(int argc, char const *argv[])
{
	int server_fd, new_socket; long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	int	opt = 1;

	// Only this line has been changed. Everything is same.

	https://en.wikipedia.org/wiki/List_of_HTTP_status_codes

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

    if (setsockopt(server_fd, SOL_SOCKET,
		SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

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
	toto.append("Content-type: text/html\n");
	toto.append("Content-Length: 1024\n\n");

	// Open and read the content store it in a string
	fd_test_html = open("home/default.html", O_RDWR);
	char buffer;
	while (read(fd_test_html, &buffer, 1))
		file_content.append(&buffer, 1);
	close(fd_test_html);
	toto.append(file_content);
	/*
	fd_test_html = open("home/test.html", O_RDWR);
	if (fd_test_html < 0)
	{
		perror("open");
		exit(-1);
	}
	*/
	//char *titi = itoa(file_content.size(), 10);
	//toto.append(std::string(titi));
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
		send_page(pars(buffer), new_socket);
		//write(new_socket , toto.c_str() , strlen(toto.c_str()));
		printf("------------------toto message sent-------------------\n");
		// Close the socket / the filedescriptor
		close(new_socket);
	}
	return 0;
}
