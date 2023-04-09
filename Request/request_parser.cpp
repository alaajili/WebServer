//
// Created by Abderrahmane Laajili on 2/15/23.
//

#include "request.hpp"

std::vector<std::string>	split_request_str(std::string request_str)
{
	std::vector<std::string>	req;
	int	end = request_str.find("\r\n");
	while (end != -1) {
		req.push_back(request_str.substr(0, end));
		request_str.erase(request_str.begin(), request_str.begin() + end + 2);
		end = request_str.find("\r\n");
        if (request_str[end+2] == '\r' && request_str[end+3] == '\n')
            break;
	}
	req.push_back(request_str.substr(0, end));
	return req;
}

std::string error_file_name() {
		return "HTTP/1.1 400 Bad Request\r\n"
			   "Server: klinix\r\n"
			   "Content-Length: 49\r\n\r\n"
			   "<h1>400 Bad Request</h1>\n"
			   "<h2>File-Name ERROR</h2>";
}

void    check_headers(client_info& client) {
	Request &request = client.request;
    request.chunked = false;
	if (request.method == NONE) {
		request.resp_headers = error_501();
		request.file_len = 0;
		client.writable = true;
		client.headers_str.done = false;
		return ;
	}

	std::map<std::string, std::string>::iterator it = request.headers.find("Transfer-Encoding");
    if (it != request.headers.end() && request.headers["Transfer-Encoding"] == "chunked")
		request.chunked = true;
	else if (it != request.headers.end() && request.headers["Transfer-Encoding"] != "chunked") {
		request.resp_headers = error_501();
		request.file_len = 0;
		client.writable = true;
		client.headers_str.done = false;
		return ;
	}
	std::map<std::string, std::string>::iterator it2 = request.headers.find("Content-Length");
	if (request.method == POST && it == request.headers.end() && it2 == request.headers.end()) {
		request.resp_headers = error_400();
		request.file_len = 0;
		client.writable = true;
		client.headers_str.done = false;
		return ;
	}
	it = request.headers.find("Host");
	if (it == request.headers.end()) {
		request.resp_headers = error_400();
		request.file_len = 0;
		client.writable = true;
		client.headers_str.done = false;
		return ;
	}
	it = request.headers.find("File-Name");
	if (request.method == POST && (it == request.headers.end() || request.headers["File-Name"].empty())) {
		request.resp_headers = error_file_name();
		request.file_len = 0;
		client.writable = true;
		client.headers_str.done = false;
		return ;
	}



}

void	parse_requests(std::list<client_info>& clients) {
    for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (it->headers_str.done && !it->headers_str.parsed) {
            std::string req_str = it->headers_str.str;
            std::vector<std::string> req;
            req = split_request_str(req_str);
            get_headers(req, it->request);
            it->headers_str.parsed = true;
            check_headers(*it);
        }
    }
}
