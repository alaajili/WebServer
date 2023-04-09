//
// Created by Abderrahmane Laajili on 1/31/23.
//

#include "parser.hpp"

void    Error_msg(const char *message)
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
        Error_msg("fail to read from server config");
    while(std::getline(file, line))
    {
        to_parse += line;
        if(!file.eof())
            to_parse += '\n';
    }
    file.close();
	if(to_parse[to_parse.size() - 1] != '\n')
		print_error("your config file should end wirh newline");
    return to_parse;
}

//////////split with string ////////////////////////////////////
std::vector<std::string> split_with_string(const std::string& to_split, const std::string delimiter)
{
    std::vector<std::string>    return_value;
    std::size_t                 start = 0;
    std::size_t                 end;
    while((end = to_split.find(delimiter, start)) != std::string::npos)
    {
        return_value.push_back(to_split.substr(start, end - start));
        start = end + delimiter.length();
    }
    return_value.push_back(to_split.substr(start));
    return return_value;
}

///////////split with character///////////////////////////////////
std::vector<std::string> split_with_char(std::string str, char delimiter)
{
    size_t start = 0;
    size_t end;
    std::vector<std::string> return_value;

    while (str[start])
    {
        while(str[start] && str[start] == delimiter)
            start++;
        if(!str[start])
            break;
        end = start;
        while(str[end] && str[end] != delimiter)
            end++;
        return_value.push_back(str.substr(start, end - start));
        start = end;
    }
    return return_value;
}

std::vector<std::string> take_strings_to_parse(const char *file_path)
{
    std::string To_parse = read_file(file_path);
    std::string delimiter = "server_name";
    std::size_t start = 0;
    std::size_t end;

    while((end = To_parse.find(delimiter, start)) != std::string::npos)
    {
        To_parse.replace(end, delimiter.length(), "name");
        start = end + delimiter.length();
    }
    std::vector<std::string> spl = split_with_string(To_parse, "server");
    spl.erase(spl.begin());
    return spl;
}