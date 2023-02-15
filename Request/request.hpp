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

enum Method {GET, POST, DELETE, NONE};

struct client_info {
	socklen_t				address_len;
	struct sockaddr_storage	address;
	int						sock;
	std::string				request;
};


struct Header {
	std::string	name;
	std::string value;
};

struct Request {
	Method	method;
	std::string	url;
	std::string	version;
	std::vector<Header>	headers;
	std::string	body;
};


void	handle_requests(std::vector<Server>& servers);
void	parse_request(const std::vector<client_info>& clients);


#endif //WEBSERVER_REQUEST_HPP
