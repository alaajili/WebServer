//
// Created by Abderrahmane Laajili on 4/8/23.
//
#include "Cgi.hpp"

int check_extension2(std::string name)
{
	if (name.substr(name.find_last_of(".") + 1) == "php" || name.substr(name.find_last_of(".") + 1) == "py")
		return (1);
	else
		return (0);
}

std::string int_to_string(int i){
	std::stringstream ss;
	ss << i;
	std::string str = ss.str();
	return str;
}

cgi::cgi(std::string p, Request &request)
{

	query = request.query;
	port = int_to_string(request.serv_block.port);

	path = p;
	cgi_pid = -1;
	pid_status = 0;
	php = "cgi/php-cgi";
	py = "/usr/local/bin/python3";
	if(request.method == POST)
		methode = "POST";
	else if (request.method == GET)
		methode = "GET";
}

cgi::~cgi()
{
}

int cgi::get_cgi_pid()
{
	return (this->cgi_pid);
}

std::string cgi::rand_file()
{
	char random[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int lenght = 5;
	srand(time(0));
	std::string name_file;
	name_file.reserve(lenght);
	for (int i = 0; i < lenght; i++)
		name_file += random[rand() % (sizeof(random) - 1)];
	return (name_file);
}

std::string cgi::outfile_path()
{
	char buff[PATH_MAX];
	getcwd(buff, PATH_MAX);
	std::string outfile_path(buff);
	outfile_path.append("/");
	outfile_path.append(outname);
	return (outfile_path);
}

void cgi::fill_env(Request &req)
{
	if (req.method == POST)
		env = new char *[14];
	else
		env = new char *[12];
	std::string s = "PATH_INFO=";
	s.append(path);
	if (!query.empty() && query.find('&') != std::string::npos && req.method == POST)
	{
		query = query.substr(0, query.size() - 1);
		s += '?';
		s.append(query);
	}
	if (query.empty() && req.method == POST && req.headers["Content-Type"] == "application/x-www-form-urlencoded\r")
		query = req.body;
	env[0] = new char[s.size() + 1];
	strcpy(env[0], s.c_str());
	s.clear();
	s = "QUERY_STRING=";
	s.append(query);
	env[1] = new char[s.size() + 1];
	strcpy(env[1], s.c_str());

	s.clear();
	s = "SERVER_PORT=";
	s.append(port);
	env[2] = new char[s.size() + 1];
	strcpy(env[2], s.c_str());

	s.clear();
	s = "REQUEST_METHOD=";
	s.append(methode);
	env[3] = new char[s.size() + 1];
	strcpy(env[3], s.c_str());

	s.clear();
	s = "SCRIPT_FILENAME=";
	s.append(path);
	env[4] = new char[s.size() + 1];
	strcpy(env[4], s.c_str());

	s.clear();
	s = "SCRIPT_NAME=";
	s.append(php);
	env[5] = new char[s.size() + 1];
	strcpy(env[5], s.c_str());

	s.clear();
	s = "REDIRECT_STATUS=200";
	env[6] = new char[s.size() + 1];
	strcpy(env[6], s.c_str());

	s.clear();
	s = "REMOTE_ADDR=";
	s.append("0.0.0.0");
	env[7] = new char[s.size() + 1];
	strcpy(env[7], s.c_str());

	s.clear();
	s = "HTTP_COOKIE=";
	s.append(req.headers["Cookie"]);
	env[8] = new char[s.size() + 1];
	strcpy(env[8], s.c_str());

	s.clear();
	s = "HTTP_USER_AGENT=";
	s.append(req.headers["User-Agent"]);
	env[9] = new char[s.size() + 1];
	strcpy(env[9], s.c_str());

	s.clear();
	s = "HTTP_HOST=";
	s.append("localhost");
	env[10] = new char[s.size() + 1];
	strcpy(env[10], s.c_str());

	if (methode == "POST")
	{
		s.clear();
		s = "CONTENT_TYPE=";
		s.append(req.headers["Content-Type"].substr(0, req.headers["Content-Type"].length()));
		env[11] = new char[s.size() + 1];
		strcpy(env[11], s.c_str());
		s.clear();
		s = "CONTENT_LENGTH=";
		s.append(req.headers["Content-Length"]);
		env[12] = new char[s.size() + 1];
		strcpy(env[12], s.c_str());

		env[13] = NULL;
	}
	else
		env[11] = NULL;
}

void cgi::exec_cgi(char **args, char **env, int fd, Request &req)
{
	lseek(out_fd, 0, SEEK_SET);
	std::cout << "mn west exec "<<out_fd << std::endl;
	cgi_pid = fork();
	if (cgi_pid == -1)
	{
		throw(fork_error());
	}
	if (cgi_pid == 0)
	{
		if (req.method == POST)
			dup2(fd, 0);

		dup2(out_fd, 1);
		if (execve(args[0], args, env) == -1) {
			// std::cout << "rachid haymchi y7wi achraf" << std::endl;
			exit(1);
		}
	}
}

void cgi::wait_for_cgi()
{
	int s_;
	int pid = waitpid(cgi_pid, &s_, WNOHANG);
	if (pid == -1)
		pid_status = ERROR;
	else if (pid != 0)
	{
		if (WIFSIGNALED(pid_status))
			pid_status = ERROR;
		else
			pid_status = DONE;
	}
	if (pid_status == DONE || pid_status == ERROR)
	{
		close(out_fd);
		close(in_fd);
	}
}

int cgi::check_extension(std::string str)
{
	if (str.substr(str.find_last_of(".") + 1) == "php")
		return (1);
	else if (str.substr(str.find_last_of(".") + 1) == "py")
		return (2);
	else
		return (0);
}

void cgi::fill_args()
{
	ext = check_extension(path);
	args = new char *[3];
	if (ext == 1)
	{
		args[0] = new char[php.size() + 1];
		strcpy(args[0], php.c_str());
	}
	else if (ext == 2)
	{
		args[0] = new char[py.size() + 1];
		strcpy(args[0], py.c_str());
	}
	else
	{
		throw(extension_error());
	}
	args[1] = new char[path.size() + 1];
	strcpy(args[1], path.c_str());
	args[2] = NULL;
}

void cgi::wait_for_tempfile_file()
{
	while (true)
	{
        std::cerr << "HERE" << std::endl;
		std::string t;
		std::fstream tempfile;
		tempfile.open("cgi/tempfile", std::ios::in);
		if(getline(tempfile, t))
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		tempfile.close();
	}
}

void cgi::wait_for_body_file()
{
	while (true)
	{

		std::fstream tempfile;
		tempfile.open("cgi/tmp_body");
		if(tempfile.good())
		{
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		tempfile.close();
	}
}

void cgi::parse_content_type(std::string str)
{
	int i;

	i = str.find("Content-type");
	while (str[i + 14] != ';')
	{
		content_type += str[i + 14];
		i++;
	}
}

std::string cgi::get_content_type()
{
	return(content_type);
}

int cgi::pick_ext()
{
	return(ext);
}

void cgi::deleat_heders()
{
	std::string str;
	std::string s;
	std::string f;
	std::fstream in;
	std::ofstream outfile;
	std::string c_type;

	wait_for_tempfile_file();
	in.open("cgi/tempfile", std::ios::in);

	if (ext == 1)
	{
		while (getline(in, str))
		{
			std::cerr << "HEY" << std::endl;
			std::cerr << str << std::endl;
			if (str != "\r")
			{
				c_type += str;
				headers.push_back(str);
				c_type += "\r\n";
			}
			else
			{
				str.clear();
				break;
			}
			str.clear();
		}
		while (getline(in, str))
		{
			f += str;
			if (in.eof())
				break;
			f += '\n';
		}
		content_type = c_type.substr(0, c_type.size() - 1);
		std::cerr << "SIZE = " << headers.size() << std::endl;
		for (size_t i=0; i < headers.size(); i++)
			std::cerr << headers[i] << std::endl;
	}
	else if (ext == 2)
	{
		while (getline(in, str))
		{
			f += str;
			if (in.eof())
				break;
			f += '\n';
		}
		content_type = "Content-Type: text/html";
	}
	outfile.open(outname, std::ios::out);
	outfile << f;
	in.close();
}

void cgi::exec(Request &req)
{
	fill_args();
	try
	{
		if (access(args[0], F_OK | X_OK) == -1)
			throw(cgi_open_error());
	}
	catch(...){
	}
	outname = "cgi/" + rand_file();
	if (req.method == POST)
	{
		in_fd = open("cgi/tmp_body", O_RDWR, 0666);
		wait_for_body_file();
		lseek(in_fd, 0, SEEK_SET);
	}
	out_fd = open("cgi/tempfile", O_CREAT | O_WRONLY | O_TRUNC, 0666);
	std::cout << "out_fd == "<<out_fd << std::endl;
	fill_env(req);
	exec_cgi(args, env, in_fd,req);
	close(out_fd);
	wait_for_cgi();

	deleat_heders();
	remove("cgi/tempfile");
	if (req.method == POST)
		remove("cgi/tmp_body");

	int i = 0;
	while (env[i]){
		delete env[i];
		i++;
	}
	i = 0;
	while (args[i]) {
		delete args[i];
		i++;
	}
	delete[] args;
	delete[] env;
}
