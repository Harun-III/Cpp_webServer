#ifndef HTTPRESPONSEBUILDER_HPP
#define HTTPRESPONSEBUILDER_HPP

#include "Response.hpp"
#include "ServerConfig.hpp"
#include "StaticFileHandler.hpp"
#include "Connection.hpp"
#include "ErrorHandler.hpp"

#include <iostream>

class ResponseBuilder {
private:
    ErrorHandler                error_handler;
    StaticFileHandler           static_handler;


public:
    ResponseBuilder(const ServerConfig& config);
    ~ResponseBuilder();

    std::string     generateDirectoryListing(const std::string& path) const;

    bool            isMethodAllowed(const std::string& method, const Location& location) const;
    Response    handleAutoIndex(const std::string& path) const;
    bool	isCgiRequest(const std::string& path, const Location& location) const;
    Response    buildResponse(Request& request);

    Response    handleRedirect(int status_code, const std::string& url) const;
    Response    handleGet(const Request& request, const Location& location);
    Response    handleDelete(std::string full_path);
    Response	handleCgi(Request& request);
};

#endif

