//
// Created by Abderrahmane Laajili on 4/6/23.
//

#include "response.hpp"

size_t  hex_to_dec(std::string hex_str) {
	size_t x;

	std::stringstream ss;
	ss << std::hex << hex_str;
	ss >> x;
	return x;
}

void    handle_chunked(client_info& client, fd_set *read_fds) {
	Request &request = client.request;
	char buff[1024];
	size_t r;
	(void) read_fds;


	if (!request.out_file.is_open()) {
		request.out_file.open("b.mp4");
		request.size_bool = false;
	} else {
		r = recv(client.sock, buff, 1024, 0);
		request.body.append(buff, r);
		request.body_len += r;
	}
	bool ok = false;
	while (request.body_len) {
		if (request.size_bool) {
			size_t a = std::min(request.chunk_size, request.body_len);
			request.out_file.write(request.body.c_str(), a);
			request.chunk_size -= a;
			request.body_len -= a;
			request.body.erase(0, a);
			if (request.chunk_size == 0) {
				request.size_bool = false;
			}
		} else {
			size_t f = request.body.find("\r\n");
			if (f == 0) {
				request.body.erase(0, 2);
				request.body_len -= 2;
			}
			else if (f != std::string::npos) {
				std::string hex_str = request.body.substr(0, f);
				request.chunk_size = hex_to_dec(hex_str);
				request.body.erase(0, f+2);
				request.body_len -= (f+2);
				if (request.chunk_size == 0) {
					ok = true;
					break ;
				}
				request.size_bool = true;
			}
			else
				break;
		}
	}
	if (ok) {
		request.resp_headers += (request.version + " 201 Created\r\n");
		request.resp_headers += ("Content-Type: text/plain\r\n");
		request.resp_headers += ("Content-Length: 8\r\n");
		request.resp_headers += "Server: klinix\r\n";
		request.resp_headers += "Connection: keep-alive\r\n\r\nuploaded";
		client.writable = true;
		request.file_len = 0;
		request.out_file.close();
	}
}

void	read_body_for_cgi(client_info& client) {
	Request &request = client.request;

	if (!request.out_file.is_open()) {

		request.out_file.open("cgi/tmp_body");
		request.cont_len = atoi(request.headers["Content-Length"].c_str());
		request.recved_bytes = 0;
		request.out_file.write(request.body.c_str(), request.body_len);
		request.recved_bytes += request.body_len;
	} else if (request.recved_bytes != request.cont_len) {
		char buff[1024];
		int r = recv(client.sock, buff, 1024, 0);
		request.out_file.write(buff, r);
		request.recved_bytes += r;
	}
	if (request.recved_bytes == request.cont_len) {

		request.ready_cgi = true;
		request.out_file.close();
	}
}

void	POST_method(client_info& client, fd_set *read_fds)
{
	Request& request = client.request;
	if (request.location.yes_no.upload_path) {
		if (!request.chunked) {
			if (!request.out_file.is_open()) {
				std::string upload_file = request.location.upload_path + "/b";
				request.out_file.open(upload_file.c_str());
				request.cont_len = atoi(request.headers["Content-Length"].c_str());
				std::cerr << "content-length: " << request.cont_len << std::endl;
				request.recved_bytes = 0;
				request.out_file.write(request.body.c_str(), request.body_len);
				request.recved_bytes += request.body_len;
			} else if (request.recved_bytes != request.cont_len) {
				char buff[1024];
				int r = recv(client.sock, buff, 1024, 0);
				request.out_file.write(buff, r);
				request.recved_bytes += r;
			}
			if (request.recved_bytes == request.cont_len) {
				request.resp_headers += (request.version + " 201 Created\r\n");
				request.resp_headers += ("Content-Type: text/plain\r\n");
				request.resp_headers += ("Content-Length: 8\r\n");
				request.resp_headers += "Server: klinix\r\n";
				request.resp_headers += "Connection: keep-alive\r\n\r\nuploaded";
				client.writable = true;
				request.file_len = 0;
				request.out_file.close();
			}
		} else {
			handle_chunked(client, read_fds);
		}
	}
    else {
		if (is_directory(request.path)) {
			if (!request.location.yes_no.index) {
				// forbidden
			}
			else {
				request.path += request.location.index;
			}
		}
		if (!request.ready_cgi) {
			read_body_for_cgi(client);
		}
		if (request.ready_cgi) {
			cgi cg(request.path, request);
			cg.exec(request);
			std::string out_path = cg.outfile_path();
			request.file.open(out_path.c_str());
			request.resp_headers =  "HTTP/1.1 200 OK\r\n";
			request.resp_headers += ("Content-Type: text/plain\r\n");
			request.file_len = get_file_len(out_path.c_str());
			std::cerr << "file len => " << request.file_len << std::endl;
			request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
			request.resp_headers += "Server: klinix\r\n";
			request.resp_headers += "Connection: " + request.headers["Connection"] + "\r\n\r\n";
			client.writable = true;
		}
    }
}
