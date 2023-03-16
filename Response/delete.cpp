
#include "GetReq.h"

std::string delete_method(const std::string& request, const std::string& document_root) {
    // Extract the HTTP method and URI from the request
    std::istringstream iss(request);
    std::string http_method, uri, http_version;
    iss >> http_method >> uri >> http_version;

    // Remove leading slashes from the URI
    uri.erase(std::remove(uri.begin(), uri.end(), '/'), uri.end());

    // Serve index.html for root directory
    if (uri.empty()) {
        uri = "index.html";
    }

    // Construct the file path by appending the URI to the document root
    std::string file_path = document_root + uri;

    // Check if the file exists
    std::ifstream file(file_path.c_str(), std::ios::binary);
    if (!file.good()) {
        // If the file does not exist, return a 404 response
        return "HTTP/1.1 404 Not Found\r\n"
               "Server: klinix\r\n";
    }

    // Close the file
    file.close();

    // Delete the file
    if (remove(file_path.c_str()) != 0) {
        // If the file cannot be deleted, return a 500 response
        return "HTTP/1.1 500 Internal Server Error\r\n"
               "Server: klinix\r\n";
    }

    // Construct the response
    std::ostringstream oss;
    oss << "HTTP/1.1 204 No Content\r\n"
           "Server: klinix\r\n"
           "Connection: close\r\n\r\n";

    return oss.str();
}