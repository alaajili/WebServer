//
// Created by Abderrahmane Laajili on 1/30/23.
//

#include "webserv.hpp"

int main(int ac, char **av)
{
    std::vector<Server> servers;

    if (ac != 2)
    {
        std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
        return 1;
    }
    servers = parse_conf_file(av[1]);

    init_sockets(servers);
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << servers[i].sock_fd << std::endl;
    }
    // handle_requests(servers);


    return 0;
}
