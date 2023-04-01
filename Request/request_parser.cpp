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

void    check_headers(Request& request) {
    request.chunked = false;
    for (size_t i = 0; i < request.headers.size(); i++) {
        if (request.headers[i].name == "Transfer-Encoding" && request.headers[i].value == "chunked") {
            request.chunked = true;
        }
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
            check_headers(it->request);
        }
    }
}
