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

Request	get_headers(std::vector<std::string> req)
{
	Request request;

	request.method = get_method(req[0]);
	request.path = get_path(req[0]);
	return request;
}
