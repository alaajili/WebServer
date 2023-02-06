//
// Created by Abderrahmane Laajili on 2/2/23.
//

#include "request.hpp"

void    init_sockets(std::vector<Server>& servers)
{

    for (size_t i = 0; i < servers.size(); i++) {
        struct sockaddr_in  address;

        servers[i].sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (servers[i].sock_fd < 0) {
            std::cerr << "socket() failed!!" << std::endl;
            exit(1);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(servers[i].port);
        memset(address.sin_zero, 0, sizeof address.sin_zero);
        if (bind(servers[i].sock_fd, (struct sockaddr *)&address, sizeof address)) {
            std::cerr << "bind() failed!!" << std::endl;
            exit(1);
        }
        if (listen(servers[i].sock_fd, 10)) {
            std::cerr << "listen() failed!!" << std::endl;
            exit(1);
        }
    }
}
