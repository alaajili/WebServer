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

bool	open_out_file(client_info& client)
{
	Request& request = client.request;

	if (request.is_cgi) {
		request.out_file.open("cgi/tmp_body");
		return true;
	}
	// you must give the file name in a request
	else if (request.headers.find("File-Name") != request.headers.end()) {
		std::string	upload_file = request.location.upload_path + "/" + request.headers["File-Name"];
		request.out_file.open(upload_file.c_str());
		return true;
	}
	// else a Forbidden 403 response will send to the client
	request.resp_headers = error_403();
	client.writable = true;
	request.file_len = 0;

	return false;
}

void	read_unchunked_body(client_info& client)
{
	Request &request = client.request;

	if (!request.out_file.is_open()) {
		request.cont_len = atoi(request.headers["Content-Length"].c_str());
        request.recved_bytes = 0;
		// check for the MAX CLIENT BODY SIZE
		if (request.serv_block.yes_or_no.max_body && request.cont_len > request.serv_block.max_body) {
			request.resp_headers = error_413();
			client.writable = true;
			request.file_len = 0;
			return;
		}
		if (!open_out_file(client))
			return;
		request.out_file.write(request.body.c_str(), request.body_len);
		request.recved_bytes += request.body_len;
	} else if (request.recved_bytes != request.cont_len) {
		char buff[1024];
		int r = recv(client.sock, buff, 1024, 0);
		request.out_file.write(buff, r);
		request.recved_bytes += r;
	}
}



bool	check_entity_too_large(client_info& client) {
	Request& request = client.request;

	if (request.serv_block.yes_or_no.max_body && request.recved_bytes > request.serv_block.max_body) {
		request.resp_headers = error_413();
		client.writable = true;
		request.file_len = 0;
		return false;
	}
	return true;
}

bool	read_chunked_body(client_info& client)
{
	Request &request = client.request;
	char buff[1024];
	int	r;

	if (!request.out_file.is_open()) {
		if (!open_out_file(client))
			return false;
		request.size_bool = false;
		request.recved_bytes = 0;
	} else {
		r = recv(client.sock, buff, 1024, 0);
		request.body.append(buff, r);
		request.body_len += r;
	}
	while (request.body_len) {
		if (request.size_bool) {
			size_t a = std::min(request.chunk_size, request.body_len);
			request.out_file.write(request.body.c_str(), a);
			request.recved_bytes += a;
			// check for the MAX CLIENT BODY SIZE everytime I write in the out_file
			if (!check_entity_too_large(client))
				return false;
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
					return true;
				}
				request.size_bool = true;
			}
			else
				break;
		}
	}
	return false;
}

void	read_body_for_cgi(client_info& client) {
	Request &request = client.request;
	if (!request.chunked) {
        read_unchunked_body(client);
		if (request.recved_bytes >= request.cont_len) {
			request.ready_cgi = true;
			request.out_file.close();
		}
	}
	else {
		if (read_chunked_body(client)) {
            request.ready_cgi = true;
            request.out_file.close();
        }
	}
}

void	created_201(Request &request) {
	request.resp_headers = (request.version + " 201 Created\r\n");
	request.resp_headers += ("Content-Type: text/plain\r\n");
	request.resp_headers += ("Content-Length: 8\r\n");
	request.resp_headers += "Server: klinix\r\n";
	request.resp_headers += "Connection: keep-alive\r\n\r\n";
    request.resp_headers += "uploaded";
}

void	run_cgi(client_info& client) {
	Request& request = client.request;

	cgi cg(request.path, request); // initialize the cgi
    try {
        cg.exec(request);
    } catch (int status) {
        if (check_error_pages(request,status))
        {
            request.path = request.serv_block.error_pages[status];
            request.file.open(request.path);
            generate_headers(request, status);
        }
        else {
            generate_error_headers(request, status);
            request.file_len = 0;
        }
        client.writable = true;
        return;
    }// execute the cgi
	request.out_path = cg.outfile_path(); // get the output file from cgi
	// then generate the response
	request.file.open(request.out_path.c_str());
	request.resp_headers =  "HTTP/1.1 200 OK\r\n";
    for (size_t i = 0; i < cg.headers.size(); i++) {
        request.resp_headers += cg.headers[i] + "\r\n";
    }
    if (request.resp_headers.find("Content-Type:") != std::string::npos)
        request.resp_headers += ("Content-Type: text/html\r\n");
	request.file_len = get_file_len(request.out_path.c_str());
	request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
	request.resp_headers += "Server: klinix\r\n";
	request.resp_headers += "Connection: " + request.headers["Connection"] + "\r\n\r\n";
	client.writable = true;
}

void	POST_method(client_info& client)
{
	Request& request = client.request;

	// if directory and location and have index file (append the index)
	if (is_directory(request.path) && request.location.yes_no.index)
		request.path += request.location.index;

	// if path is not a directory and location have cgi (handle post with cgi)
	if (!is_directory(request.path) && path_is_cgi(request)) {
        request.is_cgi = true;
		// if the body is not complete reading (read)
		if (!request.ready_cgi)
			read_body_for_cgi(client);
		// if I complete reading body (run cgi script then generate the response)
		if (request.ready_cgi) {
			run_cgi(client);
		}
	}
	// if the request location have an upload path (handle post to upload body content in a file)
	// in this case you have to send a header in the request named "File-Name" and give
	// the file u want to upload the name you want
	else if (request.location.yes_no.upload_path) {
		if (!request.chunked) {
			read_unchunked_body(client);
			// if body is complete (send response back to the client to tell him that the file uploaded)
			if (request.recved_bytes >= request.cont_len) {
				created_201(request);
				client.writable = true;
				request.file_len = 0;
				request.out_file.close();
			}
		} else {
			if (read_chunked_body(client)) {
				created_201(request);
				client.writable = true;
				request.file_len = 0;
				request.out_file.close();
			}
		}
	}
	else {
		request.resp_headers = error_403();
		request.file_len = 0;
		client.writable = true;
	}

}
