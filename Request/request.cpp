//
// Created by Abderrahmane Laajili on 2/2/23.
//

#include "request.hpp"




void    Request::clear() {
	path.clear();
    uri.clear();
	version.clear();
	query.clear();
	method = NONE;
    headers.clear();
    headers_sent = false;
    file.close();
    file_len = 0;
    sent_bytes = 0;
    matched = false;
    chunked = false;
    resp_headers.clear();
	if (!out_path.empty()) {
        remove(out_path.c_str());
    }
	out_path.clear();


	body.clear();
	body_len = 0;
	cont_len = 0;
    recved_bytes = 0;
	is_cgi = false;
	ready_cgi = false;
	size_bool = false;
}


void    RequestHeaders::clear() {
    str.clear();
    done = false;
    parsed = false;
}

client_info::client_info() {
    this->writable = false;
    request.matched = false;
}

client_info::client_info(const client_info& ci) {
    address_len = ci.address_len;
    address = ci.address;
    sock = ci.sock;
    writable = ci.writable;

    request.matched = ci.request.matched;
    request.headers_sent = ci.request.headers_sent;
    request.file_len = ci.request.file_len;
    request.body_len = ci.request.body_len;
    request.cont_len = ci.request.cont_len;
    request.size_bool = ci.request.size_bool;
    request.matched = ci.request.matched;
    request.chunked = ci.request.chunked;
    request.sent_bytes = ci.request.sent_bytes;
    request.recved_bytes = ci.request.recved_bytes;
    request.is_cgi = ci.request.is_cgi;
    request.ready_cgi = ci.request.ready_cgi;
    request.size_bool = ci.request.size_bool;


    headers_str.done = ci.headers_str.done;
    headers_str.parsed = ci.headers_str.parsed;
}

client_info&    client_info::operator=(const client_info& ci) {
    address_len = ci.address_len;
    address = ci.address;
    sock = ci.sock;
    writable = ci.writable;

    request.matched = ci.request.matched;
    request.headers_sent = ci.request.headers_sent;
    request.file_len = ci.request.file_len;
    request.body_len = ci.request.body_len;
    request.cont_len = ci.request.cont_len;
    request.size_bool = ci.request.size_bool;
    request.matched = ci.request.matched;
    request.chunked = ci.request.chunked;
    request.sent_bytes = ci.request.sent_bytes;
    request.recved_bytes = ci.request.recved_bytes;
    request.is_cgi = ci.request.is_cgi;
    request.ready_cgi = ci.request.ready_cgi;
    request.size_bool = ci.request.size_bool;

    headers_str.done = ci.headers_str.done;
    headers_str.parsed = ci.headers_str.parsed;
    return *this;
}

std::vector<int>	init_sockets(std::vector<Server>& servers)
{
	std::map<int, int>	port_sock;
	std::vector<int>	sockets;

	for (size_t i = 0; i < servers.size(); i++) {
		struct sockaddr_in  address;
        if (port_sock.find(servers[i].port) == port_sock.end()) {
            servers[i].sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (servers[i].sock_fd < 0) {
                std::cerr << "socket() failed!!" << std::endl;
                exit(1);
            }
            int value = 1;
            setsockopt(servers[i].sock_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
            fcntl(servers[i].sock_fd, F_SETFL, O_NONBLOCK);

            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(servers[i].port);
            memset(address.sin_zero, 0, sizeof address.sin_zero);

            if (bind(servers[i].sock_fd, (struct sockaddr *) &address, sizeof address)) {
                std::cerr << "bind() failed!!" << std::endl;
                exit(1);
            }
            if (listen(servers[i].sock_fd, 100)) {
                std::cerr << "listen() failed!!" << std::endl;
                exit(1);
            }
            port_sock[servers[i].port] = servers[i].sock_fd;
            sockets.push_back(servers[i].sock_fd);
        }
        else {
            servers[i].sock_fd = port_sock[servers[i].port];
        }
    }
    return sockets;
}

void	check_hanged_clients(std::list<client_info>& clients) {
	for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
		if (!it->writable && it->request.method == POST) {
			it->request.resp_headers = error_400();
			it->writable = true;
			it->request.file_len = 0;
			it->request.out_file.close();
		}
	}
}

void	wait_on_clients(const std::vector<int>& sockets,std::list<client_info>& clients,
						fd_set *read_fds, fd_set *write_fds)
{
	FD_ZERO(read_fds);
    FD_ZERO(write_fds);
	while (1) {
		int max_socket = -1;
		for (size_t i = 0; i < sockets.size(); i++) {
			FD_SET(sockets[i], read_fds);
			if (sockets[i] > max_socket)
				max_socket = sockets[i];
		}
		for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
    	    if (it->writable)
    	        FD_SET(it->sock, write_fds);
            else
			    FD_SET(it->sock, read_fds);
			if (it->sock > max_socket)
				max_socket = it->sock;
		}

		struct timeval tv;
		tv.tv_sec = 15;
		tv.tv_usec = 0;
        int ret = select(max_socket + 1, read_fds, write_fds, 0, &tv);
        if (ret < 0) {
            continue;
        }
        else if (ret == 0) {
			check_hanged_clients(clients);
            continue;
        }
        else {
            break;
        }
    }
}

void	accept_clients(const std::vector<int>& sockets, std::list<client_info>& clients,
					fd_set *read_fds)
{
	for (size_t i = 0; i < sockets.size(); i++) {
		if (FD_ISSET(sockets[i], read_fds)) {
			client_info new_client;
			new_client.address_len = sizeof new_client.address;
			new_client.sock = accept(sockets[i], (struct sockaddr*)&new_client.address,
					&new_client.address_len);
            fcntl(new_client.sock, F_SETFL, O_NONBLOCK);
            new_client.writable = false;
            new_client.headers_str.done = false;
            new_client.headers_str.parsed = false;
            new_client.request.clear();
			clients.push_back(new_client);
			std::cout << "\033[1;32m" << "NEW CLIENT CONNECTED marhbaaa biiiiik" << std::endl;
			std::cout << "NUMBER OF CLIENTS: " << clients.size() << "\033[0m" <<std::endl;
		}
	}
}

void	get_requests(std::list<client_info>& clients, fd_set *read_fds)
{
	for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (FD_ISSET(it->sock, read_fds) && !it->headers_str.done) {
            int r;
            char buff[1024];
            r = recv(it->sock, buff, 1024, 0);
            if (r == 0 || r == -1) {
				close(it->sock);
				std::list<client_info>::iterator it2 = it;
				it2--;
				clients.erase(it);
				it = it2;
				std::cout << "\033[1;31m" << "CLIENT DISCONNECTED!!!" << std::endl;
				std::cout << "\033[1;32m" << "NUMBER OF CLIENTS: " << clients.size() << "\033[0m" <<std::endl;
				continue;
			}
			else {
                it->headers_str.str.append(buff, r);
                size_t f = it->headers_str.str.find("\r\n\r\n");
                if (f != std::string::npos) {
                    it->headers_str.done = true;
                    int start = (f % 1024) + 4;
                    it->request.body.append(buff + start, buff + r);
                    it->request.body_len = r - start;
                }
            }
        }
    }
}

void	handle_requests(std::vector<Server>& servers)
{
	std::vector<int>		sockets;
	std::list<client_info>	clients;

	sockets = init_sockets(servers);
	while (1337)
	{

		fd_set  read_fds;
        fd_set  write_fds;
		wait_on_clients(sockets, clients, &read_fds, &write_fds);
		accept_clients(sockets, clients, &read_fds);
		get_requests(clients, &read_fds);
        parse_requests(clients);
        server_block_selection(clients, servers);
        handle_method(clients, &write_fds);
	}
}


