#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <string.h>
#include "Server.hpp"

int create_socket(cfg::Server &s) {
    int opt = 1;
	int	res;
    struct sockaddr_in6 addr;

	res = socket(AF_INET6, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (res == -1) {
		perror("socket() failed");
		return (-1);
	} else {
		s._socket = res;
	}

	res = setsockopt(s._socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
	if (res == -1) {
		perror("setsockopt(à failed");
		return (-1);
	}

	res = ioctl(s._socket, FIONBIO, (char *)&opt);
	if (res == -1) {
		perror("ioctl() failed");
		return (-1);
	}

	memset(&addr, 0, sizeof(addr));

	addr.sin6_family = AF_INET6;
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port = htons(s._listen);

	res = bind(s._socket, (struct sockaddr *)&addr, sizeof(addr));
	if (res == -1) {
		perror("bind() failed");
		return (-1);
	}

	res = listen(s._socket, 42);
	if (res == -1) {
		perror("listen() failed");
		return (-1);
	}

	return (1);
}
