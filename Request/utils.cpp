//
// Created by Abderrahmane Laajili on 2/17/23.
//

#include "request.hpp"

Method	get_method(std::string str)
{
	Method method;

	if (str.compare(0, 3, "GET") == 0)
		method = GET;
	else if (str.compare(0, 4, "POST") == 0)
		method = POST;
	else if (str.compare(0, 6, "DELETE") == 0)
		method = DELETE;
	else
		method = NONE;
	return method;
}

std::string	get_path(std::string str)
{
	std::string	path;
	size_t		i;

	i = str.find('/');
	while (str[i] != ' ') {
		path += str[i];
		i++;
	}
	return path;
}

std::string	get_version(std::string str, size_t path_len)
{
	std::string	version;
	size_t		i;

	i = str.find('/');
	i += (path_len + 1);
	while (i  < str.size()) {
		version += str[i];
		i++;
	}
	return version;
}

std::string	get_header_name(std::string str)
{
	std::string	name;
	size_t		i;

	i = 0;
	while (str[i] != ':') {
		name += str[i];
		i++;
	}
	return name;
}

std::string	get_header_value(std::string str)
{
	std::string	value;
	size_t		i;

	i = 0;
	while (str[i] != ':')
			i++;
	i+=2;
	while (i < str.size()) {
		value += str[i];
		i++;
	}
	return value;
}

Request	get_headers(std::vector<std::string> req)
{
	Request request;

	request.method = get_method(req[0]);
	request.path = get_path(req[0]);
	request.version = get_version(req[0], request.path.length());
	for (size_t i = 1; i < req.size(); i++)
	{
        Header header;
        header.name = get_header_name(req[i]);
        header.value = get_header_value(req[i]);
        request.headers.push_back(header);
	}
	return request;
}
