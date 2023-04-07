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

std::string delete_method(client_info& client)
{
	Request& request = client.request;
    std::string file_path = request.path;
    std::ostringstream oss;

    if (is_directory(file_path))
    {
        if (isAccessibleDir(file_path) == true)
        {
            removeDirectory(file_path);  
            oss << No_Content_204();
        }
        else
            oss << error_500();
    }
    else
    {
        std::ifstream file(file_path.c_str());
        if (!file.good())
            oss << error_404();
        else
        {
            if (isAccessible(file_path) == true)
            {
                removeFile(file_path);
                oss << No_Content_204();
            }
            else
                oss << error_500();
        }
        file.close();
    }
    return oss.str();
}

