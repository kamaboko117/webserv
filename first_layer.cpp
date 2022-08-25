#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>

void	display_flag_poll(void) {
	printf("test for all flag:\n");
	printf("POLLIN = %u\n", POLLIN);
	printf("POLLPRI = %u\n", POLLPRI);
	printf("POLLOUT = %u\n", POLLOUT);
	printf("POLLERR = %u\n", POLLERR);
	printf("POLLHUP = %u\n", POLLHUP);
	printf("POLLNVAL = %u\n", POLLNVAL);
	printf("POLLRDNORM = %u\n", POLLRDNORM);
	printf("POLLRDBAND = %u\n", POLLRDBAND);
	printf("POLLWRNORM = %u\n", POLLWRNORM);
	printf("POLLWRBAND = %u\n", POLLWRBAND);
	printf("POLLRDHUP = %u\n", POLLRDHUP);
}


typedef struct	ft_server {
	int	def; // 0 or 1 is it the default server or not
	std::string	host;
	int	port;
	int	server_fd;
	struct sockaddr_in address;
	int		addrlen = sizeof(address);
	int	opt = 1;
	std::string	server_name;
	long	valread;
	int		rc;

}				t_server;

std::vector<t_server> list_server;

void	create_server(t_server &s) {
	s.server_fd = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (s.server_fd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	s.address.sin_family = AF_INET;
	s.address.sin_port = htons(s.port);
	s.address.sin_addr.s_addr = INADDR_ANY;

	memset(s.address.sin_zero, 0, sizeof(s.address.sin_zero));

	if (setsockopt(s.server_fd, SOL_SOCKET,
		SO_REUSEADDR | SO_REUSEPORT, &s.opt, sizeof(s.opt))) {
		perror("setsocket");
		close(s.server_fd);
		exit(EXIT_FAILURE);
	}

	/*
	if (ioctl(s.server_fd, FIONBIO, (char *)&s.opt) == -1) {
		perror("ioctl() failed");
		close(s.server_fd);
		exit(-1);
	}
	*/

	if (bind(s.server_fd, (struct sockaddr *)&s.address, sizeof(s.address))) {
		perror("bind");
		close(s.server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(s.server_fd, 42)) {
		perror("listen");
		close(s.server_fd);
		exit(EXIT_FAILURE);
	}

}

int main(int ac, char **av) {

	struct pollfd	fds[2];
	int				p_ret;
	int				timeout = 3000;
	int				new_socket;

	memset(fds, 0, sizeof(fds));

	t_server ser1;
	t_server ser2;

	ser1.def = 1;
	ser1.host.append("test_host1");
	ser1.port = 5050;
	ser1.server_name.append("test_name1");

	ser2.def = 0;
	ser2.host.append("test_host2");
	ser2.port = 6060;
	ser2.server_name.append("test_name2");

	/* Create server */
	create_server(ser1);
	create_server(ser2);

	/* server list updated */
	list_server.push_back(ser1);
	list_server.push_back(ser2);

	std::cout << "list_server.size() = " << list_server.size() << std::endl;


	/* fds for poll */
	fds[0].fd = ser1.port;
	fds[0].events = POLLIN;

	fds[1].fd = ser2.port;
	fds[1].events = POLLIN;

	while (1) {
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");

		p_ret = poll(fds, 2, 5000);

		if (p_ret == -1) {
			perror("poll");
			exit(EXIT_FAILURE);
		} else if (p_ret == 0){
			printf("timeout\n");
		}

		if ((new_socket = accept(ser1.server_fd, (struct sockaddr *)&ser1.address,
						(socklen_t *)&ser1.addrlen)) == -1) {
			perror("accept");
			close(ser1.server_fd);
			exit(EXIT_FAILURE);
		}

		char	buffer[30000] = {0};
		ser1.valread = read(new_socket, buffer, 30000);
		printf("%s\n", buffer);

		std::string	mssg("Coucou !\n");
		send(new_socket, mssg.c_str(), mssg.size(), MSG_CONFIRM);
		close(new_socket);
		printf("-------- Message send--------\n");
	}

	return (0);
}
