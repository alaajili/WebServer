//
// Created by Abderrahmane Laajili on 4/5/23.
//

extern char **environ;
void setCGIenv(Request &rhs){
	headerToEnv();
}setCGIenv
void executeCGI(std::string path,Request &rhs){
	SetCGIenv();

	int fd = createTmpfile();
	if (fork() == 0) {
		dup2(fdclient,0);
		dup2(fd, 1);
		if (path.extension == ".php")
			execve(cgi-bin,path,environ);
		else if (path.extension == ".py")
			execve(cgi-bin,path,environ);
	}
	parseHeaderfromCgi();
	sendResponse(fd);
}