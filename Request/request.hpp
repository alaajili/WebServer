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


struct client_info {
	socklen_t				address_len;
	struct sockaddr_storage	address;
	int						sock;
	std::string				request;
};


void	handle_requests(std::vector<Server>& servers);


#endif //WEBSERVER_REQUEST_HPP
