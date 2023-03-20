//
// Created by ANASS on 2/19/2023.
//

#include "GetReq.h"

std::string error404()
{
    return "HTTP/1.1 404 Not Found\r\n"
            "Server: klinix\r\n";
}

std::string long_to_string(long num) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

std::map<std::string, std::string> init_map() {
    std::map<std::string, std::string> m;

    m[".html"] = "text/html";
    m[".htm"] = "text/html";
    m[".css"] = "text/css";
    m[".js"] = "text/javascript";
    m[".json"] = "application/json";
    m[".png"] = "image/png";
    m[".jpg"] = "image/jpeg";
    m[".jpeg"] = "image/jpeg";
    m[".gif"] = "image/gif";
    m[".svg"] = "image/svg+xml";
    m[".ico"] = "image/x-icon";
    m[".txt"] = "text/plain";
    m[".pdf"] = "application/pdf";
    m[".mp4"] = "video/mp4";

    return m;
}

std::string get_Content_type(std::string &file_path)
{
	static const std::map<std::string, std::string> extension_to_mime_type = init_map();

    std::string extension = file_path.substr(file_path.find_last_of('.'));
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    std::map<std::string, std::string>::const_iterator it = extension_to_mime_type.find(extension);
    if (it != extension_to_mime_type.end()) {
        return it->second;
    } else {
        return "application/octet-stream";
    }
}



fileData	read_fromFile(std::string file_path)
{
	std::string line;
	fileData file_data;

    std::ifstream myfile (file_path.c_str());

    file_data.len_file = 0;
    file_data.data = "";
    while (myfile.good())
    {
        char buff[1024];

        myfile.read(buff, 1024);
        //buff[myfile.gcount()] = 0;
        file_data.len_file += myfile.gcount();
        file_data.data.append(buff,myfile.gcount());

    }
    myfile.close();
    return file_data;
//    else
//    {
//        file_data.len_file = -1;
//	    return file_data;
//    }
}


void	fill_response(response &_response)
{
	_response.Content_type = "Content-Type: ";
	_response.Content_len = "Content-Length: ";
}

std::string get_method(Request &request,std::string path)
{
    response resp;
    std::string response;

	fill_response(resp);


//    path += request.path;
//    std::cerr << "path: "<< path << std::endl;
	fileData file_data = read_fromFile(path);
    request.rep_len = file_data.len_file;
    if (file_data.len_file == -1)
        return error404();
    else
    {
        resp.status_code = "HTTP/1.1 200 OK\r\n"; // hardcoded http version
	    resp.Content_type += get_Content_type(path);
	    resp.Content_len += long_to_string(file_data.len_file);
        response = resp.status_code;
        response += "Server: klinix\r\n";
        response += resp.Content_len + "\r\n";
        response += resp.Content_type + "\r\n" + "Connection: Keep-Alive\r\n\r\n";
        request.rep_len += response.length();
        response.append(file_data.data);
    }
    return response;
}

std::string handle_method(std::vector<client_info>& clients)
{
    std::string response;
	std::string location; // location ? hardcode
    for (size_t i = 0; i < clients.size(); i++) {
        for (size_t j = 0; j < clients[i].requests.size(); j++) {
            if (clients[i].requests[j].method == 0 && clients[i].requests[j].response.empty()) {
                /*----------GET------------*/
                location = clients[i].requests[j].location.root;
                location += clients[i].requests[j].path;
                location += clients[i].requests[j].location.index;
                std::cerr << "location => " << location << std::endl;
                clients[i].requests[j].response = get_method(clients[i].requests[j], location);
                clients[i].requests[j].offset = 0; /// <----------
            }
            if (clients[i].requests[j].method == 2) {

            }
        }
    }
    return response;
}

