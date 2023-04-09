
#include "parser.hpp"

int pick_index(std::string path, std::string flag){
    int i;
    if(flag == "first"){
        i = 0;
        while(path[i] && path[i] == '/'){
            if(path[i + 1] != '/')
                break;
            i++;
        }
        return i;
    }
    else if(flag == "end"){
        i = (int)path.size() - 1;
        while(i >= 0 && path[i] == '/'){
            if(path[i - 1] != '/')
                break;
            i--;
        }
        return i;
    }
    return 0; 
}

std::string change_path(std::string path){
    size_t a = 0;
    if(path == "/")
        return ("/");
    while(path[a] && path[a] == '/')
        a++;
    if(a == path.length())
        return ("/");
    int i = pick_index(path, "first");
    while(i >= 0)
    {
        path.erase(i, 1);
        i--;
    }
    path.insert(i + 1, 1 , '/');
    int j = pick_index(path, "end");
	if(path[path.size() - 1] != '/')
		return  path;
    while(path[j])
        path.erase(j, 1);
    return(path);
}