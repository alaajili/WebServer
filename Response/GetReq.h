//
// Created by ANASS on 2/19/2023.
//

#ifndef WEBSERVER_GETREQ_H
#define WEBSERVER_GETREQ_H
#include "../Request/request.hpp"
#include <algorithm>
#include <map>

struct fileData {
	std::string	data;
	int len_file;
};


class response {

    public:
        std::string Content_type;
        std::string Content_len;
        std::string status_code;
        std::string vrs_protocol;
};


#endif //WEBSERVER_GETREQ_H
