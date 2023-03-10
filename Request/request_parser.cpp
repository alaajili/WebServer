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

void	parse_requests(std::vector<client_info>& clients)
{
	for (size_t i = 0; i < clients.size(); i++) {
		if (clients[i].request.ready) {
            std::cerr << "READY TO PARSE" << std::endl;
			std::vector<std::string> req;
			req = split_request_str(clients[i].request_str);
			get_headers(req, clients[i].request);
            std::cerr << clients[i].request_str << std::endl;
            clients[i].request_str.clear();
		}
	}
}
