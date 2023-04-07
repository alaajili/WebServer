
#include <iostream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/wait.h>
#include "Cgi.hpp"

int main(int argc, char *argv[]) {
    cgi cg("/Users/ael-kouc/Desktop/cgi/a.py");
    cg.exec();
    std::string cgi_file_path = cg.get_outfile_path();
    std::cout << cgi_file_path << std::endl;
}
