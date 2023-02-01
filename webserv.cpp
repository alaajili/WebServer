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
    std::cout << servers[0].port << std::endl;
    std::cout << servers[0].host << std::endl;
    std::cout << servers[0].server_name << std::endl;


    return 0;
}
