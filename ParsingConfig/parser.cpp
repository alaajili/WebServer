//
// Created by Abderrahmane Laajili on 1/31/23.
//

#include "parser.hpp"

std::vector<Server> parse_conf_file(const char *file_path)
{
    std::vector<std::string>    st = take_strings_to_parse(file_path);
    std::vector<Server>         servers;

    for (size_t i = 0; i < st.size(); i++)
    {
        Holder holder(st[i]);
        Server serv = parse_data(holder);
        servers.push_back(serv);
    }
    return servers;
}

Location    take_location(Holder &holder)
{
    Location    loc;
    std::string id;

    while (holder.am_here && holder.am_here != '}')
    {
        id = holder.take_id();
        holder.skip_all();
        if (id == "allow_methods")
        {
            loc.yes_no.methods = true;
            loc.methods = holder.pick_methods();
        }
        else if (id == "root")
        {
            loc.yes_no.root = true;
            loc.root = holder.pick_root();
        }
        else if (id == "autoindex")
        {
            loc.yes_no.autoindex = true;
            loc.autoindex = holder.pick_autoindex();
        }
        else if (id == "index")
        {
            loc.yes_no.index = true;
            loc.index = holder.pick_index();
        }
        else
            print_error("error in location");
    }
    holder.advance();
    return (loc);
}

Server  parse_data(Holder& holder)
{
    Server      server;
    std::string id;

    holder.skip_all();
    holder.advance();
    while (holder.am_here && holder.am_here != '}')
    {
        holder.skip_all();
        id = holder.take_id();
        if (id == "port")
        {
            server.yes_or_no.port = true;
            server.port = holder.take_port();
        }
        else if (id == "host")
        {
            server.yes_or_no.host = true;
            server.host = holder.take_host();
        }
        else if (id == "error_page")
        {
            server.yes_or_no.error_pages = true;
            server.error_pages = holder.take_error_pages();
        }
        else if (id == "upload_path")
        {
            server.yes_or_no.upload_path = true;
            server.upload_path = holder.take_upload_path();
        }
        else if (id == "cgi")
        {
            server.yes_or_no.cgi = true;
            holder.take_cgi();
        }
        else if (id == "name")
        {
            server.yes_or_no.server_name = true;
            server.server_name = holder.take_server_name();
        }
        else if (id == "location")
        {
            server.yes_or_no.locations = true;
            std::string loc = holder.take_loc_path();
            holder.location[loc] = take_location(holder);
            std::cout << holder.am_here << std::endl;
        }
        else if (id == "}")
            break;
        else
            print_error("error");

    }
    if (server.yes_or_no.locations == true)
        server.location = holder.location;
    if (server.yes_or_no.cgi == true)
        server.cgi = holder.tmp;
    return server;
}
