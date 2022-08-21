#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>



int main(int ac, char **av) {
	int					server_fd;
	int					new_socket;
	long				valread;
	struct	sockaddr_in	address;
	int					addrlen = sizeof(address);
	int					opt = 1;
	int					port = 8080;

	server_fd = socket(AF_INET, SOCK_STREAM, getprotobyname("tcp")->p_proto);
	if (server_fd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = INADDR_ANY;

	memset(address.sin_zero, 0, sizeof(address.sin_zero));

	if (setsockopt(server_fd, SOL_SOCKET,
		SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsocket");
		exit(EXIT_FAILURE);
	}

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))) {
		perror("bind");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 1)) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("\n+++++++ Waiting for new connection ++++++++\n\n");

		if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
			(socklen_t *)&addrlen)) == -1) {
			perror("accept");
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		char	buffer[30000] = {0};
		valread = read(new_socket, buffer, 30000);
		printf("%s\n", buffer);

		std::string	mssg("Coucou !\n");
		send(new_socket, mssg.c_str(), mssg.size(), MSG_CONFIRM);
		close(new_socket);

		printf("-------- Message send--------\n");
	}

	return (0);
}
