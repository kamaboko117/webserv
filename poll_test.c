#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>



int main(int ac, char **av) {
	/*general value*/
	int		fd = 0;
	char	buff[11];
	int		ret = 0;

	/*poll argument*/
	struct pollfd	fds[1];
	int				p_ret;
	int				timeout = -1;


	while (1) {

		fds[0].fd = fd;
		fds[0].events = 0;
		fds[0].events |= POLLIN;


		p_ret = poll(fds, 1, timeout);

		if (p_ret == -1) {
			perror("poll");
			exit(EXIT_FAILURE);
		}

		if (p_ret == 0) {
			printf("ret = %d\n", ret);
			printf("\ttimeout\n");
		} else {

			memset((void *)buff, 0, 11);
			ret = read(fd, buff, 10);
			printf("ret = %d\n", ret);

			if (ret != -1) {
				printf("  buff = [%s]\n", buff);
			}
		}

	}
	return (0);
}
