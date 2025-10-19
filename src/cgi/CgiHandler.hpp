#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Location.hpp"
#include "Response.hpp"

class CgiHandler {
private:
    const Request&      request;
    const Location&     location;
    std::string         script_path;
    std::string         cgi_executable;

    std::string         getCgiExecutable(const std::string& file_path) const;
    std::string         getFileExtension(const std::string& path) const;
    char**		buildEnvVariables () const;

public:
    CgiHandler(const Request& req, const Location& loc);
    ~CgiHandler();

    Response            execute();
};

#endif

