//
// Created by Abderrahmane Laajili on 4/6/23.
//

#include "response.hpp"

std::string	get_file_extension(std::string path) {
	std::string extension;

	size_t f = path.find_last_of('.');
	if (f == std::string::npos)
		return "";
	for (size_t i = f; i < path.size(); i++) {
		extension += path[i];
	}
	return extension;
}

void	generate_headers(Request &request) {
	request.resp_headers = (request.version + " 200 OK\r\n");
	request.resp_headers += ("Content-Type: " + get_content_type(request.path) + "\r\n");
	request.file_len = get_file_len(request.path);
	request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
	request.resp_headers += "Server: klinix\r\n";
	request.resp_headers += "Connection: " + request.headers["Connection"] + "\r\n\r\n";
}

void    GET_method(client_info& client)
{
	Request& request = client.request;


	if (client.request.method == DELETE) // hendle delete method response like auto_index
	{
		request.resp_headers = delete_method(client);
		request.file_len = 0;
		client.writable = true;
		return;
	}
	std::cerr << "PATH: " << request.path << std::endl;
	// check if is a directory
	if (is_directory(request.path)) {
		if (request.uri[request.uri.length() - 1] != '/') {
			moved_permanently(request);
			request.file_len = 0;
			client.writable = true;
			return;
		}
		else if (request.location.yes_no.index) // if there is an index append it
			request.path += request.location.index;
		else if (request.location.autoindex == ON) {
			request.resp_headers = auto_index(request);
			request.file_len = 0;
			client.writable = true;
			return;
		}
		else {
			request.resp_headers = error_403();
			client.writable = true;
			request.file_len = 0;
			return ;
		}
	}
	std::string ext = get_file_extension(request.path);
	if (ext == ".php" || ext == ".py") {
		//TODO: handle cgi

	}
	else {
		request.file.open(request.path.c_str());
		// check if not open
		if (!request.file.is_open()) {
			request.resp_headers = error_404();
			client.writable = true;
			request.file_len = 0;
			return;
		}
		generate_headers(request);
		client.writable = true;
		client.headers_str.done = false;
	}
}
