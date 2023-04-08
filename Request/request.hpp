//
// Created by Abderrahmane Laajili on 2/2/23.
//

#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP

#include "../ParsingConfig/parser.hpp"
#include <cstring> /// for linux
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <list>
#include <iterator>


class Server;

enum Method {GET, POST, DELETE, NONE};


struct RequestHeaders {
    std::string str;
    bool        done;
    bool        parsed;

    void    clear();
};

struct Request {
	Method				    			method;
	std::string			    			path;
    std::string             			uri;
    std::string                         query;
	std::string			    			version;
	std::map<std::string, std::string>	headers;
    bool                    			headers_sent;
    std::ifstream           			file;
    std::ofstream           			out_file;
    size_t                  			file_len;
    size_t                  			sent_bytes;
    size_t                  			recved_bytes;
    std::string             			resp_headers;


    size_t                  cont_len;
    bool                    chunked;
    std::string             body;
    size_t                  body_len;
    size_t                  chunk_size;
    bool                    size_bool;


	bool ready_cgi;

    // server block and location
    Server      serv_block;
    Location    location;
    bool        matched;

    void    clear();
    size_t  get_content_length();

};

struct client_info {
	socklen_t				    address_len;
	struct sockaddr_storage	    address;
	int						    sock;
    RequestHeaders              headers_str;
	Request                     request;
    bool                        writable;

    client_info();
    client_info(const client_info& ci);
    client_info&    operator=(const client_info& ci);

};


void	handle_requests(std::vector<Server>& servers);
void	parse_requests(std::list<client_info>& clients);
void	get_headers(std::vector<std::string> req, Request& request);
void    server_block_selection(std::list<client_info>& clients, std::vector<Server> servers);
bool    is_directory(std::string path);

/*--------------*/
void    handle_method(std::list<client_info>& clients, fd_set *write_fds, fd_set *read_fds);
/*-------------*/

std::string                         error_501();
std::string                         error_400();

#endif //WEBSERVER_REQUEST_HPP
