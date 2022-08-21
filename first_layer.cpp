#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <vector>

typedef struct	ft_server {
	int	def; // 0 or 1 is it the default server or not
	std::string	host;
	int	port;
	std::string	server_name;

}				t_server;

std::vector<t_server> list_server;

int main(int ac, char **av) {

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

	list_server.push_back(ser1);
	list_server.push_back(ser2);


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
