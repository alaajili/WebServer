//
// Created by Abderrahmane Laajili on 1/31/23.
//

#include "parser.hpp"

void    Holder::check_brackets()
{
    int bracket = 0;
    for (int i = 0; server[i]; i++)
    {
        if (server[i] == '{' || server[i] == '}')
            bracket++;
    }
    if (bracket % 2 != 0)
    {
        std::cout<< "close your brackets" << std::endl;
        exit(1);
    }
}

Holder::Holder(std::string serv)
{
    server = serv;
    check_brackets();
    index = 0;
    second_index = 0;
    am_here = serv[0];
    var_to_check = 0;
}

void    Holder::advance()
{
    if(server[index + 1])
    {
        index++;
        am_here = server[index];
    }
}

void    Holder::skip_all()
{
    while(server[index] <= 32 || server[index] == '\n')
        advance();
}

void    Holder::skip_spaces()
{
    while(server[index] <= 32)
        advance();
}

void    Holder::update(size_t ind)
{
    if(server[ind])
    {
        index = ind;
        am_here = server[index];
    }
}

size_t Holder::find_eof_line()
{
    second_index = index;
    while(server[second_index] && server[second_index] != '\n')
        second_index++;
    return second_index;
}

std::string Holder::take_id()
{
    std::string return_value;
    while(am_here  && (am_here != ' ' && am_here != '\n'))
    {
        return_value += am_here;
        advance();
    }
    return return_value;
}

std::string Holder::take_id_()
{
    std::string return_value;
    while(am_here  &&  am_here != '\n')
    {
        return_value += am_here;
        advance();
    }
    return return_value;
}
// which one???

/////////////////////////////////////////////////////////
//////////////////////take_tokens////////////////////////
/////////////////////////////////////////////////////////

////////////////take port/////////////////////
bool    check_is_digit(std::string tmp)
{
    for (int i = 0; tmp[i] ; i++)
    {
        if (!isdigit(tmp[i]))
            return true;
    }
    return false;
}

size_t Holder::take_port()
{
    int         return_value;
    skip_spaces();
    std::string tmp = take_id();

    if(check_is_digit(tmp))
    {
        std::cout << "you can't insert a non digit in you port" << std::endl;
        exit(1);
    }
    for(int i = 0; tmp[i] ; i++)
    {
        if(!isdigit(tmp[i]))
        {
            std::cout<<"you can't insert a non digit in you port" << std::endl;
            exit(1);
        }
    }
    return_value = atoi(tmp.c_str());
    skip_all();
    return return_value;
}

////////////take_error_pages////////////////////
void    print_error(std::string error)
{
    std::cerr << error << std::endl;
    exit(1);
}

std::map<int, std::string>  pick_error_pages(std::vector<std::string> data)
{
    std::map<int, std::string>  error_pages;
    size_t                      counter;

    if((data.size() % 2))
        print_error("you need to insert a valid error pages");
    for(counter = 0; counter < data.size(); counter++)
    {
        if(check_is_digit(data[counter]))
            print_error("you need to insert a valid error pages");
        int number = atoi(data[counter].c_str());
        counter++;
        error_pages[number] = data[counter];
    }
    return error_pages;
}

std::map<int, std::string>  Holder::take_error_pages()
{
    std::map<int, std::string>  error_pages;
    std::string                 storing = take_id_();

    error_pages = pick_error_pages(split_with_char(storing, ' '));
    skip_all();
    return error_pages;
}

////////////////upload_path//////////////////////
std::string Holder::take_upload_path()
{
    std::string upload_path;

    skip_spaces();
    upload_path = take_id();
    skip_all();
    return upload_path;
}

////////////////cgi//////////////////////////////
void    Holder::take_cgi()
{
    skip_spaces();
    std::vector<std::string> data = split_with_char(take_id_(), ' ');
    if(data.size() != 2 || (data[1] != ".php" && data[1] != ".py"))
        print_error("CGI invalid");
    tmp[data[1]] = data[0];
    skip_all();
}

///////////////take location path//////////////////
std::string Holder::take_loc_path()
{
    skip_spaces();
    std::string path = take_id();
    skip_all();

    if(am_here != '{')
        print_error("location must be followed by {");
    second_index = index;
    while(server[second_index] != '}')
        second_index++;
    if(!server[second_index])
        print_error("location must be closed with }");
    advance();
    skip_all();
    return path;
}

///////////////pick methods//////////////////
std::vector<std::string> Holder::pick_methods()
{
    skip_spaces();
    std::string tmp = take_id_();
    std::vector<std::string> methods = split_with_char(tmp, ' ');
    if(methods.size() > (size_t)4)
        print_error("you can enter only 3 methods");
    for(size_t counter = 0; counter < methods.size(); counter++)
    {
        if(methods[counter] != "POST" && methods[counter] != "GET" && methods[counter] != "DELETE")
            print_error("methods not allowed");
    }
    skip_all();
    return methods;
}

///////////////pick root//////////////////
std::string Holder::pick_root()
{
    skip_spaces();
    std::string root = take_id();
    skip_all();
    return root;
}

///////////////pick index//////////////////
std::string  Holder::pick_index()
{
    skip_spaces();
    std::string index = take_id();
    skip_all();
    return index;
}

/////////////////take host//////////////////
std::string Holder::take_host()
{
    skip_spaces();
    std::string host = take_id();
    skip_all();
    return host;
}

///////////////take server name//////////////////
std::string Holder::take_server_name()
{
    skip_spaces();
    std::string server_name = take_id();
    skip_all();
    return server_name;
}

///////////////pick autoindex//////////////////
int Holder::pick_autoindex()
{
    int     return_value = 0;

    skip_spaces();
    std::string autoindex = take_id();
    if (autoindex == "on")
        return_value = ON;
    else if (autoindex == "off")
        return_value = OFF;
    else
        print_error("use ON or OFF only to define your autoindex");
    skip_all();
    return return_value;
}