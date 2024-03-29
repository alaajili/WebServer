//
// Created by Abderrahmane Laajili on 3/17/23.
//

#include "request.hpp"

std::string get_server_name(const std::string& str) {
    std::string server_name;

    for (size_t i = 0; i < str.length() && str[i] != ':'; i++)
        server_name += str[i];

    return server_name;
}

int get_port(const std::string& str) {
    int port;
    std::string s;

    size_t  i = 0;
    for (; i < str.length() && str[i] != ':'; i++);
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
    freeaddrinfo(res);
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

	host_value = request.headers["Host"];
    // get infos from the host header
    std::string server_name = get_server_name(host_value);
    int port = get_port(host_value);
    std::string host_ip = get_host_ip(server_name); // generate host ip

    std::vector<Server> matched_servers = host_port_match(host_ip, port, servers);
    request.serv_block = get_server_block(server_name, matched_servers);
}

bool    is_directory(std::string path) {
    struct stat info;

    stat(path.c_str(), &info);
    if (S_ISDIR(info.st_mode))
        return true;
    return false;
}


void    match_location(Request& request) {
    std::map<std::string, Location> locations = request.serv_block.location;

    request.uri = request.path;

    std::map<std::string, Location>::iterator it;
    std::vector<std::pair<std::string, Location> >  matched_locations;

    for (it = locations.begin(); it != locations.end(); it++) {
        std::string loc = it->first;
        if (request.path.length() >= loc.length() && request.path.compare(0, loc.length(), loc) == 0) {
            matched_locations.push_back(*it);
        }
    }

    request.location = matched_locations[0].second;
    size_t len = matched_locations[0].first.length();
	if (matched_locations[0].first == "/")
		request.location.root += "/";

    for (size_t i = 1; i < matched_locations.size(); i++) {
        if (matched_locations[i].first.length() > len) {
            request.location = matched_locations[i].second;
			if (matched_locations[i].first == "/")
				request.location.root += "/";
            len = matched_locations[i].first.length();
        }
    }
	 request.path.replace(0, len, request.location.root);
}

void    server_block_selection(std::list<client_info>& clients, std::vector<Server> servers) {
    for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (it->headers_str.done && !it->request.matched) {
            match_server_block(it->request, servers);
            match_location(it->request);
            it->request.matched = true;
        }
    }
}
