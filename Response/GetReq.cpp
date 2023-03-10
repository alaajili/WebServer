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
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            file_data.len_file += line.length();
            file_data.data += line;
        }
        myfile.close();
        return file_data;
    }
    else
    {
        file_data.len_file = -1;
	    return file_data;
    }
}


void	fill_response(response &_response)
{
	_response.Content_type = "Content-Type: ";
	_response.Content_len = "Content-Length: ";
}

std::string getmethod(client_info &client,std::string &path)
{
    response resp;
    std::string response;

	fill_response(resp);

    path += client.request.path;
    std::cerr << "path: "<< path << std::endl;
	fileData filedata = read_fromFile(path);
    std::cerr << filedata.data << std::endl;
    if (filedata.len_file == -1)
        return error404();
    else
    {
        resp.status_code = "HTTP/1.1 200 OK\r\n"; // hardcoded http version
	    resp.Content_type += get_Content_type(path);
	    resp.Content_len += long_to_string(filedata.len_file);
        response = resp.status_code;
        response += "Server: klinix\r\n";
        response += resp.Content_len + "\r\n";
        response += resp.Content_type + "\r\n" + "Connection: Keep-Alive\r\n\r\n";
        response += filedata.data;
    }
    return response;
}

std::string handlmethod(std::vector<client_info>& clients)
{
    std::string response;
	
	std::string location = "/Users/alaajili/Desktop/WebServer/fit-master"; // location ? hardcoded
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].request.ready)
        {
			std::cout << "method : " << clients[i].request.method << std::endl;
			if (clients[i].request.method == 0)
			{
				/*----------GET------------*/
                response = getmethod(clients[i],location);
                // std::cout << response ; // for test
                send(clients[i].sock, response.c_str(), response.length(), 0);

			}
        }
    }
    return "";
}

