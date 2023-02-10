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
	handle_requests(servers);


    return 0;
}
