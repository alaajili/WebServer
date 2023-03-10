//
// Created by Abderrahmane Laajili on 2/2/23.
//

#include "request.hpp"

std::vector<int>	init_sockets(std::vector<Server>& servers)
{
	std::map<int, int>	port_sock;
	std::vector<int>	sockets;

	for (size_t i = 0; i < servers.size(); i++) {
		struct sockaddr_in  address;

        if (port_sock.find(servers[i].port) == port_sock.end()) {
            servers[i].sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (servers[i].sock_fd < 0) {
                std::cerr << "socket() failed!!" << std::endl;
                exit(1);
            }

            fcntl(servers[i].sock_fd, F_SETFL, O_NONBLOCK);

            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(servers[i].port);
            memset(address.sin_zero, 0, sizeof address.sin_zero);

            if (bind(servers[i].sock_fd, (struct sockaddr *) &address, sizeof address)) {
                std::cerr << "bind() failed!!" << std::endl;
                exit(1);
            }
            if (listen(servers[i].sock_fd, 10)) {
                std::cerr << "listen() failed!!" << std::endl;
                exit(1);
            }
            port_sock[servers[i].port] = servers[i].sock_fd;
            sockets.push_back(servers[i].sock_fd);
        }
        else {
            servers[i].sock_fd = port_sock[servers[i].port];
        }
    }
    return sockets;
}

void	wait_on_clients(const std::vector<int>& sockets,const std::vector<client_info>& clients,
						fd_set *read_fds, fd_set *write_fds)
{
	int max_socket  = -1;

	FD_ZERO(read_fds);
	for (size_t i = 0; i < sockets.size(); i++) {
		FD_SET(sockets[i], read_fds);
		if (sockets[i] > max_socket)
			max_socket = sockets[i];
	}
	for (size_t i = 0; i < clients.size(); i++) {
        FD_SET(clients[i].sock, read_fds);
		// FD_SET(clients[i].sock, write_fds);
		if (clients[i].sock > max_socket)
			max_socket = clients[i].sock;
	}
	if (select(max_socket + 1, read_fds, write_fds, 0, 0) < 0) {
        std::cerr << "error in select()" << std::endl;
        exit(1);
    }
}

void	accept_clients(const std::vector<int>& sockets, std::vector<client_info>& clients,
					fd_set *read_fds)
{
	for (size_t i = 0; i < sockets.size(); i++) {
		if (FD_ISSET(sockets[i], read_fds)) {
			client_info new_client;
			new_client.address_len = sizeof new_client.address;
			new_client.sock = accept(sockets[i], (struct sockaddr*)&new_client.address,
					&new_client.address_len);
			clients.push_back(new_client);
		}
	}
}

void	get_requests(std::vector<client_info>& clients, fd_set *read_fds, fd_set *write_fds)
{
	for (size_t i = 0; i < clients.size(); i++) {
        clients[i].request.ready = false;
		if (FD_ISSET(clients[i].sock, read_fds)) {
			int r;
			char buff[1024];
			r = recv(clients[i].sock, buff, 1024, 0);
            if (r < 0) {
                std::cerr << "error in recv()" << std::endl;
                exit(1);
            }
            else if (r == 0) {
                std::cout << "client disconnected!!!" << std::endl;
                close(clients[i].sock);
                clients.erase(clients.begin()+i);
                continue;
            }
            else {
                clients[i].request_str.insert(clients[i].request_str.size(), buff, r);
                clients[i].request.ready = true;
                FD_SET(clients[i].sock, write_fds);
            }
		}
        if (FD_ISSET(clients[i].sock, write_fds))
            std::cerr << "WRITABLE" << std::endl;
	}
}

void	handle_requests(std::vector<Server>& servers)
{
	std::vector<int>			sockets;
	std::vector<client_info>	clients;

	sockets = init_sockets(servers);
	while (1337)
	{
		fd_set  read_fds;
        fd_set  write_fds;
		wait_on_clients(sockets, clients, &read_fds, &write_fds);
		accept_clients(sockets, clients, &read_fds);
		get_requests(clients, &read_fds, &write_fds);
        parse_requests(clients);
        std::cerr << "number of clients: " << clients.size() << std::endl;
        handlmethod(clients);
        for (size_t i = 0; i < clients.size(); i++) {
            FD_CLR(clients[i].sock, &write_fds);
        }

	}
}


