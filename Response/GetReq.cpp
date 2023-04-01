//
// Created by ANASS on 2/19/2023.
//

#include "GetReq.h"

std::string error404()
{
    return "HTTP/1.1 404 Not Found\r\n"
            "Server: klinix\r\n\r\n";
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


void    GET_response(client_info& client)
{
    if (!client.request.headers_sent) {
        send(client.sock, client.request.resp_headers.c_str(), client.request.resp_headers.length(), 0);
        client.request.sent_bytes = 0;
        client.request.headers_sent = true;
        std::cout << "\033[33m" << "start sending data to client\033[0m" << std::endl;
    }
    else {
        char buff[1024];
        client.request.file.read(buff, 1024);
        int r = client.request.file.gcount();
        int rr = send(client.sock, buff, r, 0);
        if (rr == -1) {
            std::cerr << "ERROR IN SEND" << std::endl;
            return;
        }
        client.request.sent_bytes += rr;
        if (client.request.sent_bytes == client.request.file_len) {
            client.request.clear();
            client.headers_str.clear();
            std::cerr << "\033[32m" << "GET REQUEST DONE\033[0m" << std::endl;
            client.writable = false;
        }
    }
}

void    GET_method(client_info& client)
{
    Request& request = client.request;

    std::cerr << "PATH: " << request.path << std::endl;
    request.file.open(request.path.c_str());
    if (!request.file.is_open()) {
        //TODO: throw 404
        std::cerr << "(THROW 404)" << std::endl;
        exit(404);
    }
    request.resp_headers = "";
    request.resp_headers += (request.version + " 200 OK\r\n");
    request.resp_headers += ("Content-Type: " + get_content_type(request.path) + "\r\n");
    request.file_len = get_file_len(request.path);
    request.resp_headers += ("Content-Length: " + long_to_string(request.file_len) + "\r\n");
    request.resp_headers += "Server: klinix\r\n";
    request.resp_headers += "Connection: keep-alive\r\n\r\n";
    client.writable = true;
}

size_t  hex_to_dec(std::string hex_str) {
    size_t x;

    std::stringstream ss;
    ss << std::hex << hex_str;
    ss >> x;
    return x;
}

void    handle_chunked(client_info& client, fd_set *read_fds) {
    Request &request = client.request;
    char buff[1024];
    size_t r;
    (void) read_fds;

    if (!request.out_file.is_open()) {
        request.out_file.open("b.mp4");
        request.size_bool = false;
    } else {
        r = recv(client.sock, buff, 1024, 0);
        request.body.append(buff, r);
        request.body_len += r;
    }
    bool ok = false;
    while (request.body_len) {
        if (request.size_bool) {
            size_t a = std::min(request.chunk_size, request.body_len);
            request.out_file.write(request.body.c_str(), a);
            request.chunk_size -= a;
            request.body_len -= a;
            request.body.erase(0, a);
            if (request.chunk_size == 0) {
                request.size_bool = false;
            }
        } else {
            size_t f = request.body.find("\r\n");
            if (f == 0) {
                request.body.erase(0, 2);
                request.body_len -= 2;
            }
            else if (f != std::string::npos) {
                std::string hex_str = request.body.substr(0, f);
                request.chunk_size = hex_to_dec(hex_str);
                request.body.erase(0, f+2);
                request.body_len -= (f+2);
                if (request.chunk_size == 0) {
                    ok = true;
                    break ;
                }
                request.size_bool = true;
            }
            else
                break;
        }
    }
    if (ok) {
        request.resp_headers += (request.version + " 201 Created\r\n");
        request.resp_headers += ("Content-Type: text/plain\r\n");
        request.resp_headers += ("Content-Length: 8\r\n");
        request.resp_headers += "Server: klinix\r\n";
        request.resp_headers += "Connection: keep-alive\r\n\r\nuploaded";
        client.writable = true;
        request.out_file.close();
    }
}

void	POST_method(client_info& client, fd_set *read_fds)
{
    Request& request = client.request;

    if (!request.chunked) {
        if (!request.out_file.is_open()) {
            request.out_file.open("im.mp4");
            request.cont_len = request.get_content_length();
            std::cerr << "content-length: " << request.cont_len << std::endl;
            request.recved_bytes = 0;
            size_t f = client.headers_str.str.find("\r\n\r\n");
            std::string body = client.headers_str.str.substr(f + 4);
            request.out_file.write(body.c_str(), body.length());
            request.recved_bytes += body.length();
        } else if (request.recved_bytes != request.cont_len) {
            char buff[1024];
            int r = recv(client.sock, buff, 1024, 0);
            request.out_file.write(buff, r);
            request.recved_bytes += r;
        }
        if (request.recved_bytes == request.cont_len) {
            request.resp_headers += (request.version + " 201 Created\r\n");
            request.resp_headers += ("Content-Type: text/plain\r\n");
            request.resp_headers += ("Content-Length: 8\r\n");
            request.resp_headers += "Server: klinix\r\n";
            request.resp_headers += "Connection: keep-alive\r\n\r\nuploaded";
            client.writable = true;
            request.out_file.close();
        }
    }
    else {
        handle_chunked(client, read_fds);
    }
}

void    handle_method(std::list<client_info>& clients, fd_set *write_fds, fd_set *read_fds)
{
    for (std::list<client_info>::iterator it = clients.begin(); it != clients.end(); it++) {
        client_info &client = *it;
        if (FD_ISSET(client.sock, write_fds)) {
            if (client.request.method == GET)
                GET_response(client);
            if (client.request.method == POST) {
                send(client.sock, client.request.resp_headers.c_str(), client.request.resp_headers.length(), 0);
                client.writable = false;
            }
        }
        else if (client.headers_str.done) {
            if (client.request.method == GET)
                GET_method(client);
            if (client.request.method == POST)
                POST_method(client, read_fds);
        }
    }
}

