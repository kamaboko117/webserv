#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <poll.h>
#include <vector>

#define FALSE	0
#define TRUE	1

/* global value */
int				timeout	 = (2 * 60 * 1000);		// 2min before poll timed out and return 0
int				opt = 1;						// for setsockopt() and ioctl()
int				end_server = FALSE;				// ending the server in case of failure or error
int				ger;							// global return error
int				new_fd;
int				len;							// nimber of bytes read/write
struct	pollfd	fds[128];						// struct of fd on what poll will get
int				nfds = 2;


/* server struct */
typedef struct t_server {
	int						port;				// Port on which the server will listen
	char					buffer[1024];
	int						er;					// Error Return;
	int						init_sock;			// Socket from the server
	int						new_sock;			// fd return by accept
	struct	sockaddr_in6	addr;
}				t_server;

/* -------- Creation of a server with a port on argument -------- */
int	create_server(int port, t_server &s) {

	/* Creating the socket */
	s.port = port;

	s.init_sock = socket(AF_INET6, SOCK_STREAM, getprotobyname("tcp")->p_proto);

	if (s.init_sock == -1) {
		perror("socket() failed");
		return (-1);
	}

	/* Allow socket to be reusable */
	s.er = setsockopt(s.init_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	if (s.er == -1) {
		perror("setsockopt() failed");
		close(s.init_sock);
		return (-1);
	}

	/* Setting the socket to be nonblocking */
	s.er = ioctl(s.init_sock, FIONBIO, (char *)&opt);

	if (s.er == -1) {
		perror("ioctl() failed");
		close(s.init_sock);
		return (-1);
	}

	/* setting the address for binding */

	memset(&s.addr, 0, sizeof(s.addr));

	s.addr.sin6_family	= AF_INET6;
	memcpy(&s.addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	s.addr.sin6_port	= htons(s.port);

	s.er = bind(s.init_sock, (struct sockaddr *)&s.addr, sizeof(s.addr));

	if (s.er == -1) {
		perror("bind() failed");
		close(s.init_sock);
		return (-1);
	}

	s.er = listen(s.init_sock, 42);

	if (s.er == -1) {
		perror("listen() failed");
		close(s.init_sock);
		return (-1);
	}

	return (1);

}

struct pollfd;

/* -------- CHECK_FD -------- */

int	check_fd(struct pollfd *fds, int i, std::vector<t_server> list) {
	for (int l = 0; l < list.size(); l++) {
		if (list[l].init_sock == fds[i].fd)
			return (list[l].init_sock);
	}
	return (0);
}

int main(int ac, char **av) {
	// pars(av[1]);
	//
	// I will hard code 2 server for testing the code

	int				current_size;

	std::vector<t_server> l_serv;

	t_server s1;
	t_server s2;

	create_server(8080, s1);
	create_server(5050, s2);

	l_serv.push_back(s1);
	l_serv.push_back(s2);

	memset(fds, 0, sizeof(fds));

	fds[0].fd = s1.init_sock;
	fds[0].events = POLLIN;

	fds[1].fd = s2.init_sock;
	fds[1].events = POLLIN;

	do {
		
		printf("Waiting on poll() on port %d\n", l_serv[0].port);

		ger = poll(fds, nfds, timeout);

		if (ger == -1) {
			perror("poll() failed");
			break;
		}

		if (ger == 0) {
			perror("poll() timeout");
			break;
		}

		current_size = nfds;

		for (int i = 0; i < current_size; i++) {
			
			if (fds[i].revents == 0)
				continue;

			if (fds[i].revents != POLLIN) {
				printf("Error revents = %d\n", fds[i].revents);
				end_server = TRUE;
				break;
			}

			if (fds[i].fd == l_serv[0].init_sock) {
				
				do {
					
					l_serv[0].new_sock = accept(l_serv[0].init_sock, NULL, NULL);

					if (l_serv[0].new_sock < 0) {
						if (l_serv[0].new_sock != EWOULDBLOCK) {
							perror("accept() failed");
							end_server = TRUE;
						}
						break;
					}

					fds[nfds].fd = l_serv[0].new_sock;
					fds[nfds].events = POLLIN;
					nfds++;


				} while (l_serv[0].new_sock != -1);

			} else {
				
				do {
					
					ger = recv(fds[i].fd, l_serv[0].buffer, sizeof(l_serv[0].buffer), MSG_DONTWAIT);


					if (ger < 0) {
						if (errno != EWOULDBLOCK) {
							perror("recv() failed");
							end_server = TRUE;
						}
						break;
					}

					if (ger == 0) {
						printf("Connection closed");
						close(fds[i].fd);
						break;
					}

					len = ger;

					ger = send(fds[i].fd, l_serv[0].buffer, len, MSG_CONFIRM);

					if (ger < 0) {
						perror("send() failed");
						break;
					}

					close(fds[i].fd);
					break;

				} while (1);

			}

		}

	} while (end_server == FALSE);

	return (0);
}
