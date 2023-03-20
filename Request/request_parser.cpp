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

void	parse_requests(std::vector<client_info>& clients) {
    for (size_t i = 0; i < clients.size(); i++) {
        for (size_t j = 0; j < clients[i].requests_str.size(); j++) {
            std::string req_str = clients[i].requests_str[j];
            std::vector<std::string> req;
            req = split_request_str(req_str);
            Request request;
            std::cerr << "{\n" << req_str << "}" << std::endl;
            get_headers(req, request);
            clients[i].requests.push_back(request);
        }
         clients[i].requests_str.clear();
    }
}
