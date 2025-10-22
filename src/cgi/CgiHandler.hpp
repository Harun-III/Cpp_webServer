#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Location.hpp"
#include "Response.hpp"
#include <string>

#define CGI_TIMEOUT 10  // timeout for CGI execution
#define BUFFERREADSIZE 5000

class CgiHandler {
private:
    const Request&      request;
    const Location&     location;
    std::string         script_path;
    std::string         cgi_executable;

    std::string         getCgiExecutable(const std::string& file_path) const;
    std::string         getFileExtension(const std::string& path) const;
    char**		buildArguments() const;
    char**		buildEnvVariables () const;
    void		freeEnvArray(char** env) const;
    bool                waitForCgiWithTimeout(pid_t pid, int* status) const;
    std::string		extractBody(const std::string& cgi_output) const;
    void                parseHeaders(const std::string& cgi_output, Response& response) const;


public:
    CgiHandler(const Request& req, const Location& loc);
    ~CgiHandler();

    Response            execute();
};

#endif

