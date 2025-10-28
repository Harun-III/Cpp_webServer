#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Location.hpp"
#include "Response.hpp"
#include <string>

#define CGI_TIMEOUT 5  // timeout for CGI execution in seconds

#define CGI_STRING_MAX_OUTPUT 100000
#define CGI_BUFFER 10000

class CgiHandler {
private:
	const Request&      request;
	const Location&     location;
	std::string         script_path;
	std::string         cgi_executable;
	time_t              start_time;

	int                 pid;
	cgi_e               cgi_status;

	char**              env;
	char**              args;
	
	int                 output_fd;    // File descriptor for output file

	size_t				getCgiFileLength(const std::string pathToCgiFile, size_t headSize) const;
	std::string         getCgiExecutable(const std::string& file_path) const;
	std::string         getFileExtension(const std::string& path) const;
	char**              buildArguments() const;
	char**              buildEnvVariables() const;
	void                freeEnvArray(char** env) const;
	void                parseHeaders(std::string& cgi_output, Response& response) const;
	std::string         generateOutputFilename() const;

public:
	CgiHandler(const Request& req, const Location& loc);
	~CgiHandler();
	
	std::string			cgi_output;
	void                execute(Response& response);
};

#endif

