//
// Created by Abderrahmane Laajili on 2/2/23.
//

#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP

#include "../ParsingConfig/parser.hpp"
#include <cstring> /// for linux
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>



class Server;

enum Method {GET, POST, DELETE, NONE};

struct Header {
	std::string	name;
	std::string value;
};

struct Request {
	Method				method;
	std::string			path;
	std::string			version;
	std::vector<Header>	headers;
	std::string			body;
	bool				ready;
	std::string			response;
	size_t				rep_len;
	bool				done;
	std::string::size_type  offset;

    // server block and location
    Server      serv_block;
    Location    location;
};

struct client_info {
	socklen_t				address_len;
	struct sockaddr_storage	address;
	int						sock;
	std::string				request_str;
	Request					request;
};


void	handle_requests(std::vector<Server>& servers);
void	parse_requests(std::vector<client_info>& clients);
void	get_headers(std::vector<std::string> req, Request& request);
void    server_block_selection(std::vector<client_info>& clients, std::vector<Server> servers);

/*--------------*/
std::string handlmethod(std::vector<client_info>& clients);
/*-------------*/

#endif //WEBSERVER_REQUEST_HPP
