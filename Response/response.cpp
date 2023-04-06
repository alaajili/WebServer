//
// Created by ANASS on 2/19/2023.
//

#include "response.hpp"

void    send_response(client_info& client)
{
    if (!client.request.headers_sent) {
        send(client.sock, client.request.resp_headers.c_str(), client.request.resp_headers.length(), 0);
        client.request.sent_bytes = 0;
        client.request.headers_sent = true;
        std::cout << "\033[1;33m" << "start sending data to client\033[0m" << std::endl;
    }
    else {
		if (client.request.file_len) {
			char buff[1024];
			client.request.file.read(buff, 1024);
			int r = client.request.file.gcount();
			int rr = send(client.sock, buff, r, 0);
			if (rr == -1) {
				std::cerr << "ERROR IN SEND" << std::endl;
				return;
			}
			client.request.sent_bytes += rr;
		}
		if (client.request.sent_bytes == client.request.file_len) {
            client.request.clear();
            client.headers_str.clear();
            std::cerr << "\033[1;32m" << "TRANSFER DATA DONE\033[0m" << std::endl;
            client.writable = false;
        }
    }
}

void    GET_method(client_info& client)
{
    Request& request = client.request;


	std::cerr << "PATH: " << request.path << std::endl;
	// check if is a directory
    if (is_directory(request.path)) {
		if (request.uri[request.uri.length() - 1] != '/') {
			moved_permanently(request);
			request.file_len = 0;
			client.writable = true;
			return;
		}
        else if (request.location.yes_no.index) // if there is an index append it
            request.path += request.location.index;
		else if (request.location.autoindex == ON) {
			request.resp_headers = auto_index(request);
			request.file_len = 0;
			client.writable = true;
			return;
		}
		else {
			request.resp_headers = error_403();
			client.writable = true;
			request.file_len = 0;
			return ;
		}
    }
	// if is a file try to open it
	request.file.open(request.path.c_str());
	// check if not open
    if (!request.file.is_open()) {
		std::cerr << "\33[1;31m(FILE NOT FOUND) | PATH ERROR : " << request.path << "\033[0m" << std::endl;
		request.resp_headers = error_404();
		client.writable = true;
		request.file_len = 0;
		return ;
    }
    request.resp_headers = "";
    request.resp_headers += (request.version + " 200 OK\r\n");
    request.resp_headers += ("Content-Type: " + get_content_type(request.path) + "\r\n");
    request.file_len = get_file_len(request.path);
    request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
    request.resp_headers += "Server: klinix\r\n";
    request.resp_headers += "Connection: keep-alive\r\n\r\n";
    client.writable = true;
	client.headers_str.done = false;
}


void    handle_method(std::list<client_info>& clients, fd_set *write_fds, fd_set *read_fds)
{
    for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
        client_info &client = *it;
        if (FD_ISSET(client.sock, write_fds)) {
			send_response(client);
        }
        else if (client.headers_str.done) {
            if (client.request.method == GET) {
				GET_method(client);
			}
            if (client.request.method == POST) {
				POST_method(client, read_fds);
			}
        }
    }
}

