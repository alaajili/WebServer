//
// Created by ANASS on 2/19/2023.
//

#ifndef WEBSERV_RESPONSE_H
#define WEBSERV_RESPONSE_H
#include "../Request/request.hpp"
#include <algorithm>
#include <dirent.h>
#include <map>
#include "../cgi/Cgi.hpp"



std::string							long_to_string(size_t num);
std::string							auto_index(Request& request);
void    							GET_method(client_info& client);
void								POST_method(client_info& client);
void                                delete_method(client_info& client);
std::map<int, std::string>          init_map_status();
std::string 						error_403();
std::string 						error_404();
std::string                         error_405();
std::string                         error_500();
std::string                         error_501();
std::string                         error_400();
std::string							error_413();
std::string                         No_Content_204();
void								moved_permanently(Request &request);
std::string							long_to_string(size_t num);
std::map<std::string, std::string>	init_map();
std::string							get_content_type(std::string &file_path);
size_t								get_file_len(std::string path);
bool                                check_error_pages(Request& request,int status);
void                                generate_headers(Request &request, int status);
bool                                method_allowed(client_info& client,std::string M);
std::string							get_file_extension(std::string path);
bool								path_is_cgi(Request& request);
void                            	moved_permanently_return(Request &request, std::string url);
void                                generate_error_headers(Request &request, int status);

#endif //WEBSERV_RESPONSE_H
