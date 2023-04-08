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




void    handle_method(std::list<client_info>& clients, fd_set *write_fds, fd_set *read_fds)
{
    for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
        client_info &client = *it;
        if (FD_ISSET(client.sock, write_fds)) {
			send_response(client);
        }
        else if (client.headers_str.done) {
            if (client.request.method == GET) {    // hendle delete method response like auto_index
				GET_method(client);
			}
            if (client.request.method == POST) {
				POST_method(client, read_fds);
			}
            if (client.request.method == DELETE){
                delete_method(client);
            }
        }
    }
}

