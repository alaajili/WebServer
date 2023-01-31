//
// Created by Abderrahmane Laajili on 1/30/23.
//

#include "webserv.hpp"

int main(int ac, char **av)
{
    if (ac > 2)
    {
        std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
        return 1;
    }

    return 0;
}
