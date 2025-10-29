#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

# include "Core.hpp"
# include "Request.hpp" 

class Response;

#define CGI_TIMEOUT 5  // timeout for CGI execution in seconds

#define CGI_STRING_MAX_OUTPUT 100000
#define CGI_BUFFER 10000

class CgiHandler {
private:
	std::string         script_path;
	std::string         cgi_executable;
	time_t              start_time;

	int                 pid;
	cgi_e               cgi_status;

	char**              env;
	char**              args;

	std::string			output_file;
	std::string			cgi_output;

	size_t				getCgiFileLength(const std::string pathToCgiFile, size_t headSize) const;
	std::string         getCgiExecutable(Location& location, const std::string& file_path) const;
	std::string         getFileExtension(const std::string& path) const;
	char**              buildArguments() const;
	char**              buildEnvVariables(Request& request) const;
	void                freeEnvArray(char** env) const;
	void                parseHeaders(std::string& cgi_output, Response& response) const;
	std::string         generateOutputFilename() const;

public:
	CgiHandler();
	~CgiHandler();

	std::ifstream		readFile;
	cgi_e               getStatus() const;
	void				setStatus(cgi_e status);

	void                execute(Request& request);
	void				checkProcess(Response& response);
	void                processOutput(Response& response);
};

#endif
