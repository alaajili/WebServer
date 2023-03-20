//
// Created by Abderrahmane Laajili on 3/17/23.
//

#include "request.hpp"

std::string get_server_name(const std::string& str) {
    std::string server_name;

    for (size_t i = 0; str[i] != ':'; i++)
        server_name += str[i];

    return server_name;
}

int get_port(const std::string& str) {
    int port;
    std::string s;

    size_t  i = 0;
    for (; str[i] != ':'; i++);
    i++;
    for (; i < str.length(); i++) {
        s += str[i];
    }
    port = atoi(s.c_str());
    return port;
}

std::string get_host_ip(std::string server_name) {
    std::string host_ip;
    char ip_str[INET_ADDRSTRLEN];
    void *addr;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(server_name.c_str(), 0, &hints, &res);
    struct sockaddr_in *ip = reinterpret_cast<struct sockaddr_in*>(res->ai_addr);
    addr = &(ip->sin_addr);
    inet_ntop(AF_INET, addr, ip_str, sizeof ip_str);
    host_ip += ip_str;
    return host_ip;
}

std::vector<Server> host_port_match(std::string host, int port, std::vector<Server> servers) {
    std::vector<Server> matched;
    for (size_t i = 0; i < servers.size(); i++) {
        Server serv = servers[i];
        if (serv.host == host && serv.port == port) {
            matched.push_back(serv);
        }
    }
    if (!matched.size()) {
        for (size_t i = 0; i < servers.size(); i++) {
            Server serv = servers[i];
            if (serv.host == "0.0.0.0" && serv.port == port) {
                matched.push_back(serv);
            }
        }
    }
    return matched;
}

Server  get_server_block(std::string server_name, std::vector<Server> matched) {
    Server serv;

    serv = matched[0];
    for (size_t i = 0; i < matched.size(); i++) {
        if (matched[i].server_name == server_name) {
            serv = matched[i];
            break;
        }
    }

    return serv;
}

void    match_server_block(Request& request, std::vector<Server> servers) {
    std::string host_value;
    (void)servers;
    for (size_t i = 0; i < request.headers.size(); i++) {
        if (request.headers[i].name == "Host") {
            host_value = request.headers[i].value;
            break;
        }
    }
    // get infos from the host header
    std::string server_name = get_server_name(host_value);
    int port = get_port(host_value);
    std::string host_ip = get_host_ip(server_name); // generate host ip
    std::cerr << "server name: " << server_name << std::endl;
    std::cerr << "port: " << port << std::endl;
    std::cerr << "host ip: " << host_ip << std::endl;

    std::vector<Server> matched_servers = host_port_match(host_ip, port, servers);
    request.serv_block = get_server_block(server_name, matched_servers);
    std::cerr << "BINGO: " << request.serv_block.location.size() << std::endl;
}

void    match_location(Request& request) {
    std::map<std::string, Location> locations = request.serv_block.location;

    std::map<std::string, Location>::iterator it;
    for (it = locations.begin(); it != locations.end(); it++) {
        if (it->first == request.path) {
            request.location = it->second;
            break;
        }
    }
}

void    server_block_selection(std::vector<client_info>& clients, std::vector<Server> servers) {
    for (size_t i = 0; i < clients.size(); i++) {
        for (size_t j = 0; j < clients[i].requests.size(); j++) {
            match_server_block(clients[i].requests[j], servers);
            match_location(clients[i].requests[j]);
            std::cerr << clients[i].requests[j].location.root << std::endl;
        }
    }
}
