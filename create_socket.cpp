#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat/h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>

#define	PORT	8080
#define	IPv4	AF_INET
#define	IPv6	AF_INET6

typedef struct	t_socket {
	int	server_fd;
	int	new_socket;
	struct	sockaddr_in	address;
	int	addrelen = sizeof(address);
}				t_socket;

void	create_socket(t_socket* sck) {
	sck->server_fd = socket(IPv4, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (sck->server_fd == -1) {
		perror("In socket");
		exit(EXIT_FAILURE);
	}
	sck->address.sin_family = IPv4;
	sck->address.sin_port = htons(PORT);
	sck->address.sin_addr.s_addr = INADDR_ANY;

	memset(sck->address.sin_zero, 0, sizeof(sck->address.sin_zero));

	if (bind(sck->server_fd, (struct sockaddr *)&sck->address, sizeof(sck->address))) {
		perror("In bind");
		close(sck->server_fd);
		exit(EXIT_FAILURE);
	}
	if (listen(sck->server_fd, 1)) {
		perror("in listen");
		exit(EXIT_FAILURE);
	}
}
