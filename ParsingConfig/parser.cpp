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
        if (id == "allow_methods" && loc.yes_no.methods == false)
        {
            loc.yes_no.methods = true;
            loc.methods = holder.pick_methods();
        }
        else if (id == "root" && loc.yes_no.root == false)
        {
            loc.yes_no.root = true;
            loc.root = holder.pick_root();
            loc.root = change_path(loc.root);
        }
        else if (id == "autoindex" && loc.yes_no.autoindex == false)
        {
            loc.yes_no.autoindex = true;
            loc.autoindex = holder.pick_autoindex();
        }
        else if (id == "index" && loc.yes_no.index == false)
        {
            loc.yes_no.index = true;
            loc.index = holder.pick_index();
        }
		else if (id == "upload_path" && loc.yes_no.upload_path == false)
		{
			loc.yes_no.upload_path = true;
			loc.upload_path = holder.take_upload_path();
            loc.upload_path = change_path(loc.upload_path);
		}
		else if (id == "cgi")
		{
			loc.yes_no.cgi = true;
			holder.take_cgi();
		}
        else if (id == "return" && loc.yes_no.return_ == false){
            loc.yes_no.return_ = true;
            holder.skip_spaces();
            loc.return_ = holder.take_id();
            // std::cout << server.return_ << std::endl;
            holder.skip_all();
        }
        else
            print_error("error in location");
    }
	if (loc.yes_no.cgi)
		loc.cgi = holder.tmp;
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
        if (id == "port" && server.yes_or_no.port == false)
        {
            server.yes_or_no.port = true;
            server.port = holder.take_port();
        }
        else if (id == "host" && server.yes_or_no.host == false)
        {
            server.yes_or_no.host = true;
            server.host = holder.take_host();
        }
        else if (id == "error_page" && server.yes_or_no.error_pages == false)
        {
            server.yes_or_no.error_pages = true;
            server.error_pages = holder.take_error_pages();
        }
        else if (id == "name" && server.yes_or_no.server_name == false)
        {
            server.yes_or_no.server_name = true;
            server.server_name = holder.take_server_name();
        }
        else if (id == "location")
        {
            server.yes_or_no.locations = true;
            std::string loc = holder.take_loc_path();
            loc = change_path(loc);
            holder.location[loc] = take_location(holder);
        }
        else if(id == "max_body" && server.yes_or_no.max_body == false){
            server.yes_or_no.max_body = true;
            server.max_body = holder.take_port();
        }
        else if (id == "}")
            break;
        else
            print_error("error");

    }
    if (server.yes_or_no.locations)
        server.location = holder.location;
    return server;
}
