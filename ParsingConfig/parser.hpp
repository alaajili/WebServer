//
// Created by Abderrahmane Laajili on 1/31/23.
//

#ifndef WEBSERVER_PARSER_HPP
#define WEBSERVER_PARSER_HPP


#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <map>
#include <cstdlib>
#include <cctype>

#define ON 1
#define OFF 2


struct bool_server {
    bool port;
    bool host;
    bool error_pages;
    bool upload_path;
    bool server_name;
    bool locations;
    bool max_body;
};

struct bool_location {
    bool methods;
    bool return_;
    bool root;
    bool autoindex;
    bool index;
    bool path;
	bool upload_path;
	bool cgi;

};

class Location
{
public:
    bool_location               yes_no;
    std::vector<std::string>    methods;
    std::string                 root;
    int                         autoindex;
    std::string                 index;
	std::string                 upload_path;
	std::vector<std::string>  cgi;
    std::string                         return_;
    Location();
};

class Server
{
public :
    bool_server                         yes_or_no;
    int                                 port;
    std::string                         host;
    std::string                         server_name;
    size_t                        		max_body;
    std::map<int, std::string>          error_pages;
    std::map<std::string, std::string>  cgi;
    std::map<std::string, Location>     location;

    int                                 sock_fd;

    Server();
};

////////////////////////////////////////////
////////////////holder class ///////////////
////////////////////////////////////////////
class Holder
{
public:
    std::string                         server;
    size_t                              index;
    char                                am_here;
    size_t                              second_index;
    size_t                              var_to_check;
    std::vector<std::string>  tmp;
    std::map<std::string, Location>     location;

    Holder(std::string serv);

    /////utils//////////////
    void                        check_brackets();
    size_t                      find_eof_line();
    void                        advance();
    void                        skip_all();
    void                        skip_spaces();
    void                        update(size_t ind);
    std::string                 take_id();
    std::string                 take_id_();

    //////server///////////
    size_t                      take_port();
    std::string                 take_host();
    std::map<int, std::string>  take_error_pages();
    std::string                 take_upload_path();
    void                        take_cgi();
    std::string                 take_server_name();

    ////////location//////
    std::string                 take_loc_path();
    std::vector<std::string>    pick_methods();
    std::string                 pick_root();
    std::string                 pick_index();
    int                         pick_autoindex();

};


////////////////////////////////////////////
#define GO_TO_DEFAULT
/////////////////////////////////


void                        Error_msg(const char *message);
std::string                 read_file(const char *file_path);
std::vector<std::string>    split_with_string(const std::string& to_split, const std::string delimiter);
std::vector<std::string>    take_strings_to_parse(const char *file_path);
Server                      parse_data(Holder& holder);
std::vector<std::string>    split_with_char(std::string str, char delimiter);
bool                        check_is_digit(std::string tmp);
void                        print_error(std::string error);
std::vector<Server>         parse_conf_file(const char *file_path);
int                         pick_index(std::string path, std::string flag);
std::string                 change_path(std::string path);

#endif //WEBSERVER_PARSER_HPP
