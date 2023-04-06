#include "GetReq.h"



bool isAccessible(const std::string& path)
{
    int result = access(path.c_str(), W_OK);
    if (result == 0)
        return true;
    else
        return false;
}

bool isAccessibleDir(const std::string& path)
{
    if (!is_directory(path))
    {
        if (!isAccessible(path))
            return false;
        else
            return true;
    }

    DIR* dir = opendir(path.c_str());
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string entryPath = path + "/" + entry->d_name;

        if (entry->d_type == DT_DIR) {
            if (entry->d_name[0] != '.') {
                if (!isAccessibleDir(entryPath) && access(entryPath.c_str(), X_OK)) {
                    closedir(dir);
                    return false;
                }
            }
        }
        else {
            // check file
            if (!isAccessible(entryPath)) {
                closedir(dir);
                return false;
            }
        }
    }
    closedir(dir);
    return true;
}

// std::string delete_method(Request &request) {
    
//     std::string file_path = request.path;
//     std::ostringstream oss;

//     std::ifstream file(file_path.c_str(), std::ios::binary);
//     if (!file.good()) {
//         oss << "HTTP/1.1 404 Not Found\r\n"
//                "Server: klinix\r\n";
//     }

//     file.close();

//     if (remove(file_path.c_str()) != 0) {
//         oss << "HTTP/1.1 500 Internal Server Error\r\n"
//                "Server: klinix\r\n";
//     }
//     oss << "HTTP/1.1 204 No Content\r\n"
//            "Server: klinix\r\n"
//            "Connection: close\r\n\r\n";

//     return oss.str();
// }

std::string auto_index(Request& request) /// TODO print just the path requested not the full path (root + path)
{
    std::string directory = request.path;
    std::stringstream ss;
    ss << "<html>\n"
       << "<head>\n"
	   << "<title>Index of " << request.uri << "</title>\n"
	   << "<style>\n"
	   << "table, th, td {\n"
	   << "border: 1px solid black;}\n"
	   << "</style>\n"
	   << "</head>\n"
       << "<body>\n"
       << "<h1>Index of " << request.uri << "</h1>\n"
       << "<table>\n"
       << "<tr><th>  Name  </th><th>  Last Modified  </th><th>  Size  </th></tr>\n";

    DIR* dir = opendir(directory.c_str());
    if (dir != NULL)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            std::string name(entry->d_name);
//            if (name != "." && name != "..")
//            {
                std::string path = directory + "/" + name;

                struct stat file_stat;
                if (stat(path.c_str(), &file_stat) == 0)
                {
                    char last_modified[80];
                    strftime(last_modified, 80, "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
                    std::string size;
                    if (S_ISREG(file_stat.st_mode))
                    {
                        std::stringstream size_ss;
                        size_ss << file_stat.st_size;
                        size = size_ss.str();
                    }
                    ss << "<tr><td><a href=\"" << name << "\">" << name << "</a></td><td>" <<
						last_modified << "</td><td>" << size << "</td></tr>\n";
//                }
            }
        }
        closedir(dir);
    }

    ss << "</table>\n"
       << "</body>\n"
       << "</html>\n";

    std::string response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + long_to_string(ss.str().size()) + "\r\n";
    response += "Connection: keep-alive\r\n";
    response += "\r\n";
    response += ss.str();

    return response;
}