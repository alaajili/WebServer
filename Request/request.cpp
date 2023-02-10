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

void	handle_requests(std::vector<Server>& servers)
{
	std::vector<int>	sockets;

	sockets = init_sockets(servers);
}
