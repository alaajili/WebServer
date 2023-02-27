//
// Created by ANASS on 2/19/2023.
//

#include "GetReq.h"

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
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            file_data.len_file += line.length();
            file_data.data += line;
        }
        myfile.close();
    }
	return file_data;
}


void	fill_response(response &_response)
{
	_response.Content_type = "Content-Type: ";
	_response.Content_len = "Content-Length: ";
}



void handlmethod(std::vector<client_info>& clients)
{
	response resp;


	fill_response(resp);

	std::string path = "/home/anasselb/WebServer/fit-master"; // location ?

    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].request.ready)
        {
			std::cout << "method : " << clients[i].request.method << std::endl;
			if (clients[i].request.method == 0)
			{
				/*----------GET------------*/
				// std::cout << "path : " << clients[i].request.path << std::endl;
				path += clients[i].request.path;
				fileData filedata = read_fromFile(path);
				std::cout << "path : " << path << std::endl;
				resp.Content_type += get_Content_type(path);
				resp.Content_len += itoa(filedata.len_file);
				std::cout << "Content_type : " << resp.Content_type << std::endl;
				/*------------------------*/
			}
        }
    }
}

