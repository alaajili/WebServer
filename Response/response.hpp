//
// Created by ANASS on 2/19/2023.
//

#ifndef WEBSERV_RESPONSE_H
#define WEBSERV_RESPONSE_H
#include "../Request/request.hpp"
#include <algorithm>
#include <dirent.h>
#include <map>

class cgi;


std::string							long_to_string(size_t num);
std::string							auto_index(Request& request);
void    							GET_method(client_info& client);
void								POST_method(client_info& client, fd_set *read_fds);
void                                delete_method(client_info& client);
std::map<int, std::string>          init_map_status();
std::string 						error_404();
std::string 						error_403();
std::string                         error_500();
std::string                         No_Content_204();
void								moved_permanently(Request &request);
std::string							long_to_string(size_t num);
std::map<std::string, std::string>	init_map();
std::string							get_content_type(std::string &file_path);
size_t								get_file_len(std::string path);
bool                                check_error_pages(Request& request,int status);
void                                generate_headers(Request &request, int status);

#endif //WEBSERV_RESPONSE_H
