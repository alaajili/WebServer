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
    if (request.resp_headers.find("Content-Type:") != std::string::npos)
        request.resp_headers += ("Content-Type: text/html\r\n");
	request.file_len = get_file_len(out);
	request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
	request.resp_headers += "Server: klinix\r\n";
	request.resp_headers += "Connection: " + request.headers["Connection"] + "\r\n\r\n";
}

void    generate_error_headers(Request &request, int status) {
    std::map<int,std::string> m = init_map_status();
    request.resp_headers = (request.version +  " " + m[status] + "\r\n");
    request.resp_headers += ("Content-Type: text/html\r\n");
    request.resp_headers += ("Content-Length: " + long_to_string(m[status].length()+9) + "\r\n");
    request.resp_headers += "Server: klinix\r\n";
    request.resp_headers += "Connection: " + request.headers["Connection"] + "\r\n\r\n";
    request.resp_headers += ( "<h1>" + m[status] + "</h1>" );
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


	if (!method_allowed(client,"GET"))
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
		else if (request.location.yes_no.autoindex && request.location.autoindex == ON) {
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
        try {
            cg.exec(request);
        } catch (int status) {
            std::cerr << "caught: " << status << std::endl;
            if (check_error_pages(request,status))
            {
                request.path = request.serv_block.error_pages[status];
                request.file.open(request.path);
                generate_headers(request,default_status);
            }
            else {
                generate_error_headers(request, status);
                request.file_len = 0;
            }
            client.writable = true;
            return;
        }
		request.out_path = cg.outfile_path();
		request.file.open(request.out_path.c_str());
		generate_headers_for_cgi(request, request.out_path, cg);
		client.writable = true;
		client.headers_str.done = false;
    }
	else {
		request.file.open(request.path.c_str());
		if (!request.file.is_open()) {
			if (check_error_pages(request,404))
			{
				request.path = request.serv_block.error_pages[404];
                request.file.open(request.path);
				default_status = 404;
			}
			else{
				request.resp_headers = error_404();
				client.writable = true;
                client.headers_str.done = false;
				request.file_len = 0;
				return;
			}
		}
        else if (!request.file.good()) {
            if (check_error_pages(request,500))
            {
                request.path = request.serv_block.error_pages[500];
                request.file.open(request.path);
                default_status = 500;
            }
            else{
                request.resp_headers = error_500();
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
