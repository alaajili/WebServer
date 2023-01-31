//
// Created by Abderrahmane Laajili on 1/31/23.
//

#include "parser.hpp"

void    Error_mssg(const char *message)
{
    std::cout << message << std::endl;
    exit(1);
}

std::string read_file(const char *file_path)
{
    std::string to_parse;
    std::string line;

    std::fstream file(file_path);
    if(file.fail())
        Error_mssg("fail to read from server config");
    while(std::getline(file, line))
    {
        to_parse += line;
        if(!file.eof())
            to_parse += '\n';
    }
    file.close();
    return to_parse;
}

//////////split with string ////////////////////////////////////
std::vector<std::string> split_with_string(const std::string& to_split, const std::string delimiter)
{
    std::vector<std::string>    return_value;
    std::size_t                 start = 0;
    std::size_t                 end = 0;
    while((end = to_split.find(delimiter, start)) != std::string::npos)
    {
        return_value.push_back(to_split.substr(start, end - start));
        start = end + delimiter.length();
    }
    return_value.push_back(to_split.substr(start));
    return return_value;
}

///////////split with character///////////////////////////////////
std::vector<std::string> split_with_char(std::string str, char delemiter)
{
    size_t start = 0;
    size_t end = 0;
    std::vector<std::string> return_value;

    while (str[start])
    {
        while(str[start] && str[start] == delemiter)
            start++;
        if(!str[start])
            break;
        end = start;
        while(str[end] && str[end] != delemiter)
            end++;
        return_value.push_back(str.substr(start, end - start));
        start = end;
    }
    return return_value;
}

std::vector<std::string> take_strings_to_parse(const char *file_path)
{
    std::string To_parse = read_file(file_path);
    std::string delemiter = "server_name";
    std::size_t start = 0;
    std::size_t end = 0;

    while((end = To_parse.find(delemiter, start)) != std::string::npos)
    {
        To_parse.replace(end, delemiter.length(), "name");
        start = end + delemiter.length();
    }
    std::vector<std::string> spl = split_with_string(To_parse, "server");
    spl.erase(spl.begin());
    return spl;
}