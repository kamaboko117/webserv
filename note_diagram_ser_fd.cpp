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
			continue;
		if (fds[i].revents != POLLIN) {
			"ERROR revents";
			end_server = TRUE;
			break;
		}

		/* Faire quelque chose ici pour voir quelle fd est dans la list*/
		/* On sais que poll a renvoyé quelque chose :) */

		/* Faire un comparatif de test de tous les socket a écouté avec celui de fds[i] */
		/* retourne 1 si trouvé 0 si rien */
		if (fds[i].fd == srv_fd) {
			"Listening socket is READABLE";
			do {
				new_fd = accept(srv_fd, NULL, NULL);
				if (new_fd < 0) {
					if (new_fd != EWOULDBLOCK) {
						perror("acept() failed");
						end_server = TRUE;
					}
					break;
				}

				/* ADDING the new incoming connection to the pollfd struct */
				fds[nfds].fd = new_fd;
				fds[nfds].events = POLLIN;
				nfds++;
			} while (new_fd != -1)
		} else {
			do {
				rc = recv(fds[i].fd, buffer);
				if (rc < 0)
					"ERROR";

				if (rc == 0) {
					/* Connection closed */
					break;
				}

				len_readed = rc;
				rc = send(fds[i].fd, buffer, len, 0);
				if (rc < 0) {
					/*ERROR*/
					break;
				}
			} while (TRUE);
		}
	}
} while (end_server == FALSE)
