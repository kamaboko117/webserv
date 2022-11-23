#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "Server.hpp"
#include "Create_socket.hpp"
#include "ft_itoa_string.hpp"
#include <vector>
#include "cgi_handler.hpp"
#include "utils.hpp"

#define SERVER_PORT1	12345
#define SERVER_PORT2	8080

#define TRUE             1
#define FALSE            0

int	check_init_sock(int comp, std::vector<cfg::Server> s, int server_len) {
	for (int i = 0; i < server_len; i++) {
		if (comp == s[i]._socket)
			return (s[i]._socket);
	}
	return (-1);
}

void	usage(char *str) {
	printf("Usage:\n%s [fichier de configuration]\n", str);
	exit(EXIT_FAILURE);
}

void	print_revents(struct pollfd fds) {
	if (fds.revents == POLLIN)
		return;
	if (fds.revents & POLLIN)
		printf("1 POLLIN\n");
	else
		printf("0 POLLIN\n"); 
	if (fds.revents & POLLOUT)
		printf("1 POLLOUT\n");
	else
		printf("0 POLLOUT\n"); 
	if (fds.revents & POLLPRI)
		printf("1 POLLPRI\n");
	else
		printf("0 POLLPRI\n"); 
	if (fds.revents & POLLERR)
		printf("1 POLLERR\n");
	else
		printf("0 POLLERR\n"); 
	if (fds.revents & POLLHUP)
		printf("1 POLLHUP\n");
	else
		printf("0 POLLHUP\n"); 
	if (fds.revents & POLLRDHUP)
		printf("1 POLLRDHUP\n");
	else
		printf("0 POLLRDHUP\n"); 
	if (fds.revents & POLLNVAL)
		printf("1 POLLNVAL\n");
	else
		printf("0 POLLNVAL\n"); 
}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
	}
	std::string page_upload;

	std::vector<cfg::Server>	server_list;
	try {
		server_list = getServers(argv[1]);
	} catch (std::exception &error) {
		std::cout << "Error: " << error.what() << std::endl;
		return (-1);
	}

	struct pollfd fds[server_list.size() * 200];

	memset(fds, 0, sizeof(fds));

	int	len = 0;
	for (size_t i = 0; i < server_list.size(); i++) {
		if (create_socket(server_list[i]) != -1) {
			len++;
			fds[i].fd = server_list[i]._socket;
			fds[i].events = POLLIN;
		}
	}

	/* Aucun port n'a pu être aloué */
	if (len == 0)
		return (EXIT_FAILURE);
	int		server_len = server_list.size();
	int		rc, on = 1;
	int		new_sd = -1;
	int		end_server = FALSE, compress_array = FALSE;
	int		close_conn;
	char	buffer[1048576];
	int		nfds = server_len, current_size = 0, i, j;

	std::cout << "webserv [" << GRN "start" NC << "]" << std::endl;

	do
	{
		rc = poll(fds, nfds, -1);

		if (rc < 0)
		{
			perror("  poll() failed");
			break;
		}

		if (rc == 0)
		{
			printf("  poll() timed out.  End program.\n");
			break;
		}


		current_size = nfds;
		for (i = 0; i < current_size; i++)
		{
			if(fds[i].revents == 0)
				continue;
			if (fds[i].revents & POLLERR) {
				//printf("POLLERR\n");
				close_conn = TRUE;
				close(fds[i].fd);
				fds[i].fd = 0;
				break;
			} else if(fds[i].revents & POLLHUP) {
				//printf("POLLHUP\n");
				close_conn = TRUE;
				close(fds[i].fd);
				fds[i].fd = 0;
				break;
			} else if (fds[i].revents & POLLNVAL) {
				//printf("POLLNVAL\n");
				break;
			}
			else if(fds[i].revents != POLLIN)
			{
				print_revents(fds[i]);
				//printf("  Error! revents = %d\n", fds[i].revents);
				end_server = TRUE;
				break;

			}
			if (check_init_sock(fds[i].fd, server_list, server_len) != -1)
			{
				int	ok_fd = check_init_sock(fds[i].fd, server_list, server_len);

				do
				{
					new_sd = accept(ok_fd, NULL, NULL);
					if (new_sd < 0)
					{
						if (errno != EWOULDBLOCK)
						{
							perror("  accept() failed");
							end_server = TRUE;
						}
						break;
					}
					ioctl(new_sd, FIONBIO, (char *)&on);

					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;

				} while (new_sd != -1);
			}
			else
			{
				close_conn = FALSE;

				do
				{
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0)
						break;

					std::string client;
					client.insert(client.size(), buffer, rc);
					if (rc == 0)
					{
						close_conn = TRUE;
						break;
					}
					else{
						std::string responseCGI = requestHandler(client, server_list);
						
						rc = send(fds[i].fd, responseCGI.c_str(), responseCGI.size(), 0);
						if (rc < 0)
						{
							perror("  send() failed");
							close_conn = TRUE;
							break;
						}
					}

				} while(TRUE);

				if (close_conn)
				{
					close(fds[i].fd);
					fds[i].fd = -1;
					compress_array = TRUE;
				}


			}  /* End of existing connection is readable             */
		} /* End of loop through pollable descriptors                */

		if (compress_array)
		{
			compress_array = FALSE;
			for (i = 0; i < nfds; i++)
			{
				if (fds[i].fd == -1)
				{
					for(j = i; j < nfds; j++)
					{
						fds[j].fd = fds[j+1].fd;
					}
					i--;
					nfds--;
				}
			}
		}

	} while (end_server == FALSE); /* End of serving running.    */

	for (i = 0; i < nfds; i++)
	{
		if(fds[i].fd >= 0)
			close(fds[i].fd);
	}
}
