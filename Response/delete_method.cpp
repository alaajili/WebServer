//
// Created by ANASS EL BACHA
//

#include "response.hpp"

bool isAccessible(const std::string& path)
{
    int result = access(path.c_str(), W_OK);
    if (result == 0)
        return true;
    else
        return false;
}

bool isAccessibleDir(const std::string& path)
{

    DIR* dir = opendir(path.c_str());
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string entryPath = path + "/" + entry->d_name;

        if (entry->d_type == DT_DIR) {
            if (entry->d_name[0] != '.') {
                if (!isAccessibleDir(entryPath) && access(entryPath.c_str(), X_OK)) {
                    closedir(dir);
                    return false;
                }
            }
        }
        else {
            if (!isAccessible(entryPath)) {
                closedir(dir);
                return false;
            }
        }
    }
    closedir(dir);
    return true;
}

bool removeFile(const std::string& path) {
    if (remove(path.c_str()) == 0) {
        return true;
    } else {
        return false;
    }
}

bool removeDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        std::string entryPath = path + "/" + entry->d_name;
        struct stat statbuf;
        if (stat(entryPath.c_str(), &statbuf) == -1)
            return false;
        if (S_ISDIR(statbuf.st_mode)) {
            if (!removeDirectory(entryPath)) {
                return false;
            }
        } else {
            if (!removeFile(entryPath)) {
                return false;
            }
        }
    }
    closedir(dir);
    if (rmdir(path.c_str()) == 0) {
        return true;
    } else {
        return false;
    }
}

void    delete_method(client_info& client)
{
	Request&            request = client.request;
    std::string         file_path = request.path;
    int                 default_status;

    if (!method_allowed(client,"DELETE"))
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
    if (is_directory(file_path))
    {
        if (isAccessibleDir(file_path))
        {
            removeDirectory(file_path);  
            request.resp_headers = No_Content_204();
			client.writable = true;
			request.file_len = 0;
            std::cerr << "HERE !!" << std::endl;
			return;
        }
        else{
            if (check_error_pages(request,500)){
				request.path = request.serv_block.error_pages[500];
				default_status = 500;
			}
            else{
                request.resp_headers = error_500();
			    client.writable = true;
			    request.file_len = 0;
			    return;
            }
        }
    }
    else{
        std::ifstream file(file_path.c_str());
        if (!file.good() && isAccessible(file_path)){
			if (check_error_pages(request,404)){
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
        else{
            if (isAccessible(file_path)){
                removeFile(file_path);
                request.resp_headers = No_Content_204();
                client.writable = true;
                request.file_len = 0;
                return;
            }
            else{
                if (check_error_pages(request,500)){
			    	request.path = request.serv_block.error_pages[500];
			    	default_status = 500;
			    }
                else{
                    request.resp_headers = error_500();
			        client.writable = true;
			        request.file_len = 0;
			        return;
                }
            }
        }
        file.close();
    }
	generate_headers(request,default_status);
	client.writable = true;
	client.headers_str.done = false;
}

