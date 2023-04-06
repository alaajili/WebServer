//
// Created by Abderrahmane Laajili on 1/31/23.
//

#include "parser.hpp"

Server::Server()
{
    yes_or_no.port = false;
    yes_or_no.host = false;
    yes_or_no.error_pages = false;
    yes_or_no.cgi = false;
    yes_or_no.server_name = false;
    yes_or_no.locations = false;
}

Location::Location()
{
    yes_no.methods = false;
    yes_no.root = false;
    yes_no.autoindex = false;
    yes_no.index = false;
    yes_no.path = false;
	yes_no.upload_path = false;
}
