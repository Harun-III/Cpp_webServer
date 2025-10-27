#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Location.hpp"
#include "Response.hpp"
#include <string>

#define CGI_TIMEOUT 5  // timeout for CGI execution in seconds

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
	
	std::string         output_file;  // Path to CGI output file
	int                 output_fd;    // File descriptor for output file

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

	void                execute(Response& response);
};

#endif

