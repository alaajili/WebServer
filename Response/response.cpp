//
// Created by ANASS on 2/19/2023.
//

#include "response.hpp"

bool    method_allowed(client_info& client,std::string M)
{
    std::vector<std::string>::iterator it;
    if (client.request.location.methods.empty())
        return true;
    it = std::find(client.request.location.methods.begin(),client.request.location.methods.end(), M);
    if (it != client.request.location.methods.end())
        return true;
    return false;
}

void    send_response(client_info& client, bool& to_drop)
{
    if (!client.request.headers_sent) {
        int rr = send(client.sock, client.request.resp_headers.c_str(), client.request.resp_headers.length(), 0);
		if (rr == -1 || rr == 0) {
			close(client.sock);
			to_drop = true;
			return;
		}
        client.request.sent_bytes = 0;
        client.request.headers_sent = true;
    }
    else {
		if (client.request.file_len) {
			char buff[1024];
            client.request.file.read(buff, 1024);
            int r = client.request.file.gcount();
            int rr = send(client.sock, buff, r, 0);
            if (rr == -1 || rr == 0) {
                close(client.sock);
                to_drop = true;
                return;
            }
            client.request.sent_bytes += rr;
        }
		if (client.request.sent_bytes == client.request.file_len) {
            client.request.clear();
            client.headers_str.clear();
            client.writable = false;
        }
    }
}

void    handle_method(std::list<client_info>& clients, fd_set *write_fds)
{
    for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
        client_info &client = *it;
		bool to_drop = false;
        if (FD_ISSET(client.sock, write_fds)) {
			send_response(client, to_drop);
        }
        else if (client.headers_str.done) {
            if (client.request.location.yes_no.return_)
            {
                moved_permanently_return(client.request, client.request.location.return_);
                client.request.file_len = 0;
		        client.writable = true;
            }
            else if (client.request.method == GET) {
				GET_method(client);
			}
            if (client.request.method == POST) {
				POST_method(client);
			}
            else if (client.request.method == DELETE){
                delete_method(client);
            }
        }
		if (to_drop) {
			std::list<client_info>::iterator tmp = it;
			tmp--;
			clients.erase(it);
			it = tmp;
		}
    }
}

