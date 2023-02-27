//
// Created by ANASS on 2/19/2023.
//

#include "GetReq.h"


void handlmethod(std::vector<client_info>& clients)
{
    for (size_t i = 0; i < clients.size(); i++) {
        if (clients[i].request.ready == false) {
            std::cout << "method " << clients[i].request.method << std::endl;
            if (clients[i].request.method != 0) {
                std::cout << "test --- -";
                // Open the file
                std::string filename = "../fit-master/index.html";
                std::ifstream file (filename.c_str()); /////hadrcoded path
                if (!file.is_open()) {
                    // Handle file not found error // error page 404
                    return;
                }
                // Send the response headers
                std::ostringstream response_stream;
                response_stream << "HTTP/1.1 200 OK\r\n";
                response_stream << "Content-Type: " << "text/html" << "\r\n"; /// hardcoded Content type for test
                response_stream << "Content-Length: " << "1000" << "\r\n";
                response_stream << "\r\n";
                std::string headers = response_stream.str();
                send(clients[i].sock, headers.c_str(), headers.size(), 0);

                // Send the response body
                char buffer[1024];
                int bytes_sent = 0;
                while (bytes_sent < 1000) {/// file size {
                    int bytes_to_send = std::min(1000 - bytes_sent, 1024); //file size
                    file.read(buffer, bytes_to_send);
                    int bytes_sent_now = send(clients[i].sock, buffer, bytes_to_send, 0);
                    if (bytes_sent_now == -1) {
                        // Handle send error
                        return;
                    }
                    bytes_sent += bytes_sent_now;
                }
            }
        }
    }
}

