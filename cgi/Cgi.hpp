
#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include<string>
#include "../Request/request.hpp"

class cgi
{

private:
    //int body_existence;
    std::string path;
    int cgi_pid;
    int in_fd;
    std::string methode;
    int pid_status;
    char **env;
    char **args;
    std::string out_path;
    int out_fd;
    std::string py;
    std::string query;
    std::string content_type;
    std::string php;
    std::string outname;
    int ext;
    std::string port;
    // Request req;

public:
    enum
    {
        NONE = 0,
        DONE = 1,
        ERROR = -1
    };

    cgi(std::string p, Request &request);
    ~cgi();
    void exec_cgi(char **args, char **env, int fd, Request &req);
    int get_cgi_pid();
    void wait_for_cgi();
    void fill_env(Request &req);
    void exec(Request &req);
    int check_extension(std::string str);
    void fill_args();
    std::string rand_file();
    std::string outfile_path();
    void deleat_heders();
    void wait_for_tempfile_file();
    void parse_content_type(std::string str);
    std::string get_content_type();
    int pick_ext();
    void wait_for_body_file();
    class fork_error : public std::exception
    {
        const char *what() const throw()
        {
            return ("fork failed");
        }
    };
    class cgi_open_error : public std::exception
    {
        const char *what() const throw()
        {
            return ("CGI open failed");
        }
    };
    class extension_error : public std::exception
    {
        const char *what() const throw()
        {
            return ("Unknown extension");
        }
    };
};



#endif