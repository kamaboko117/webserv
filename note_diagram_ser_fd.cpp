int	srv_fd = -1;

srv_fd = socket();

setsocketopt(srv_fd);

ioctl(srv_fd);

memset(addr);
memcpy() && port && family;

listen(srv_fd);

memset(0, fds);

fds[0].fd =  srv_fd;
fds[0].events = POLLIN;

pour cahque iteration de socket créé enterieurement 
list_server.size() = nfds;
do {
	"Waiting for incoming socket via poll\n";

	/* Attente d'une connection sur un port specifique; */
	poll(fds, nfds, timeout);

	if (poll() return < 0)
		Il y a une erreur;

	if (poll() return == 0)
		timeout de poll et break de la grande do while();
	int	current_size = nfds;
	for (int i = 0; i < current_size; i++) {
		if (fds[i].revents == 0)
			continue; // looping back to for() or do while()
		if (fds[i].revents != POLLIN) {
			"ERROR revents";
			end_server = TRUE;
			break;
		}

		if (fds[i].fd == srv_fd) {
			"Listening socket is READABLE";
			do {
				new_fd = accept(srv_fd, NULL, NULL);
				if (new_fd < 0) {
					if (errno != EWOULDBLOCK) {
						perror("acept() failed");
						end_server = TRUE;
					}
					break;
				}
				ioctl(srv_fd);

				/* ADDING the new incoming connection to the pollfd struct */
				fds[nfds].fd = new_fd;
				fds[nfds].events = POLLIN;
				nfds++;
			} while (new_fd != -1)
		} else {
			do {
				rc = recv(fds[i].fd, buffer);
				if (rc < 0) {
					if (errno != EWOULDBLOCK) {
						perror("recv() failed");
						close_conn = TRUE;
					}
				}

				if (rc == 0) {
					/* Connection closed */
					close_conn = TRUE;
					break;
				}

				len_readed = rc;
				rc = send(fds[i].fd, buffer, len, 0);
				if (rc < 0) {
					perror("send() failed");
					close_conn = TRUE;
					break;
				}
			} while (TRUE);

			if (close_conn) {
				close(fds[i].fd);
				fds[i].fd = -1;
				compress_array = TRUE;
			}
		}
	}
	if (compress_array) {
		compress_array = FALSE;
		for (i = 0; i < nfds; i++) {
			if (fds[i].fd == -1) {
				for (j = i; j < nfds; j++) {
					fds[j].fd = fds[j + 1].fd;
				}
				i--;
				nfds--;
			}
		}
	}
} while (end_server == FALSE)
