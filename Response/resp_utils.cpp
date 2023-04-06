//
// Created by Abderrahmane Laajili on 4/6/23.
//

#include "response.hpp"

std::string error_404()
{
	return "HTTP/1.1 404 Not Found\r\n"
		   "Server: klinix\r\n"
		   "Content-Length: 22\r\n\r\n"
		   "<h1>404 Not Found</h1>";
}

std::string error_403() {
	return "HTTP/1.1 403 Forbidden\r\n"
		   "Server: klinix\r\n"
		   "Content-Length: 22\r\n\r\n"
		   "<h1>403 Forbidden</h1>";
}

void	moved_permanently(Request &request) {
	request.resp_headers = "HTTP/1.1 301 Moved Permanently\r\n";
	request.resp_headers += "Location: " + request.uri + "/\r\n";
	request.resp_headers += "Content-Length: 0\r\n\r\n";
}

std::string long_to_string(size_t num) {
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

std::string get_content_type(std::string &file_path)
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

size_t  get_file_len(std::string path) {
	struct stat st;

	stat(path.c_str(), &st);
	return st.st_size;
}
