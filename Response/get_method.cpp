//
// Created by Abderrahmane Laajili on 4/6/23.
//

#include "response.hpp"

bool	check_error_pages(Request& request,int status)
{
	struct stat file_stat;
	if (request.serv_block.error_pages.find(status) != request.serv_block.error_pages.end()
					&& !stat(request.serv_block.error_pages[status].c_str(), &file_stat) && S_ISREG(file_stat.st_mode))
	{
		return true;
	}
	return false;
}

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

void	generate_headers(Request &request, int status)
{
	std::map<int,std::string> m = init_map_status();
	request.resp_headers = (request.version +  " " + m[status] + "\r\n");
	request.resp_headers += ("Content-Type: " + get_content_type(request.path) + "\r\n");
	request.file_len = get_file_len(request.path);
	request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
	request.resp_headers += "Server: klinix\r\n";
	request.resp_headers += "Connection: " + request.headers["Connection"] + "\r\n\r\n";
}

void	generate_headers_for_cgi(Request &request, std::string out, cgi cg) {
	request.resp_headers = "HTTP/1.1 200 OK\r\n";
	for (size_t i = 0; i < cg.headers.size(); i++) {
		request.resp_headers += cg.headers[i] + "\r\n";
	}
	// request.resp_headers += ("Content-Type: text/html\r\n");
	request.file_len = get_file_len(out);
	request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
	request.resp_headers += "Server: klinix\r\n";
	request.resp_headers += "Connection: " + request.headers["Connection"] + "\r\n\r\n";
}

bool	path_is_cgi(Request& request) {
	std::string ext = get_file_extension(request.path);
	for (size_t i = 0; i < request.location.cgi.size(); i++) {
		if (request.location.cgi[i] == ext)
			return true;
	}
	return false;
}

void	GET_method(client_info&	client)
{
	Request& request = client.request;
	int default_status = 200;


	if (method_allowed(client,"GET") == false)
	{
		if (check_error_pages(request,405))
		{
			request.path = request.serv_block.error_pages[405];
			default_status = 405;
		}
		else {
			request.resp_headers = error_405();
			client.writable = true;
			request.file_len = 0;
			return ;
		}
	}
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
			if (check_error_pages(request,403))
			{
				request.path = request.serv_block.error_pages[403];
				default_status = 403;
			}
			else {
				request.resp_headers = error_403();
				client.writable = true;
				request.file_len = 0;
				return ;
			}
		}
	}

    if (path_is_cgi(request)) {
		request.is_cgi = true;
		cgi cg(request.path, request);
		cg.exec(request);
		std::string out_path = cg.outfile_path();
		request.file.open(out_path.c_str());
		generate_headers_for_cgi(request, out_path, cg);
		client.writable = true;
		client.headers_str.done = false;
    }
	else {
		request.file.open(request.path.c_str());
		if (!request.file.is_open()) {
			if (check_error_pages(request,404))
			{
				request.path = request.serv_block.error_pages[404];
				default_status = 404;
			}
			else{
				request.resp_headers = error_404();
				client.writable = true;
				request.file_len = 0;
				return;
			}
		}
		generate_headers(request,default_status);
		client.writable = true;
		client.headers_str.done = false;
	}
}
