//
// Created by Abderrahmane Laajili on 2/18/23.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <sys/select.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
	int server_fd, new_socket, valread, activity;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello World!";
	fd_set readfds;
	int max_sd, sd;
	int max_clients = 7;
	std::vector<int> client_socket(7, 0);
	// create a socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// set up the server address and bind the socket
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	memset(address.sin_zero, '\0', sizeof address.sin_zero);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	// start listening for incoming connections
	if (listen(server_fd, 3) < 0) {
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	int count  = 0;
	// accept incoming connections and handle requests
	while(1) {
		printf("count ---> %d\n", count);
		// clear the socket set
		FD_ZERO(&readfds);

		// add the server socket to the set
		FD_SET(server_fd, &readfds);
		max_sd = server_fd;

		// add the client sockets to the set
		for (int i = 0; i < max_clients; i++) {
			sd = client_socket[i];
			if (sd > 0)
				FD_SET(sd, &readfds);
			if (sd > max_sd)
				max_sd = sd;
		}

		// wait for activity on one of the sockets
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if (activity < 0) {
			perror("select error");
			exit(EXIT_FAILURE);
		}

		// check for incoming connections
		if (FD_ISSET(server_fd, &readfds)) {
			printf("here\n");
			if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
				perror("accept failed");
				exit(EXIT_FAILURE);
			}

			printf("New connection accepted\n");
			printf("new_socket = %d\n", new_socket);
			// add new socket to client sockets array
			for (int i = 0; i < max_clients; i++) {
				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;
					break;
				}
			}
		}

		// check for incoming data on client sockets
		for (int i = 0; i < max_clients; i++) {
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds)) {
				// read the incoming request
				valread = read(sd, buffer, 1024);

				if (valread == 0) {
					// connection closed
					close(sd);
					client_socket[i] = 0;
				} else {
					// send a response
					buffer[valread] = '\0';
					printf("%s\n", buffer);
					send(sd, hello, strlen(hello), 0);
				}
			}
		}
		count++;
	}

	return 0;
}