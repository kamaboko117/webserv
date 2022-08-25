#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <>

/* global value */
int				timeout	 = (2 * 60 * 1000);		// 2min before poll timed out and return 0
struct	pollfd	fds[128];						// struct of fd on what poll will get
int				opt = 1;						// for setsockopt() and ioctl()


/* server struct */
typedef struct t_server {
	int						port;				// Port on which the server will listen
	char					buffer[1024];
	int						er;					// Error Return;
	int						init_sock;			// Socket from the server
	int						new_sock;			// fd return by accept
	struct	sockaddr_in6	addr;
}				t_server;

/* Creation of a server with a port on argument */
int	create_server(int port, t_server &s) {

	/* Creating the socket */
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
	s.er = ioctl(s.init_sock, FIONBIO, (char *)opt);

	if (s.er == -1) {
		perror("ioctl() failed");
		close(s.init_sock);
		return (-1);
	}

	/* setting the address for binding */

	memset(&s.addr, 0, sizeof(s.addr));

	s.addr.sin6_family	= AF_INET6;
	memcpy(&s.addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	s.addr.sin6_port	= htons(port);

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

}

int main(int ac, char **av) {
	// pars(av[1]);
	//
	// I will hard code 2 server for testing the code
	
	t_server s1;
	t_server s2;

	create_server(8080, s1);
	create_server(5050, s2);

	memset(fds, 0, sizeof(fds));

	fds[0].fd = s1.init_sock;
	fds[0].events = POLLIN;

	fds[1].fd = s2.init_sock;
	fds[1].events = POLLIN;



	return (0);
}


























