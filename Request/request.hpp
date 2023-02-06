//
// Created by Abderrahmane Laajili on 2/2/23.
//

#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP

#include "../ParsingConfig/parser.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Server;
//class Request {
//
//};


//void    handle_requests(const std::vector<Server>& servers);
void    init_sockets(std::vector<Server>& servers);


#endif //WEBSERVER_REQUEST_HPP
