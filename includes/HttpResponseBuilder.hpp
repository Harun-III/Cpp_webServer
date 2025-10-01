#ifndef HTTPRESPONSEBUILDER_HPP
#define HTTPRESPONSEBUILDER_HPP

#include "HttpResponse.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/StaticFileHandler.hpp"
#include "../includes/Client.hpp"
#include <string>
#include <map>

class HttpResponseBuilder {
private:
    const ServerConfig&         server_config;
    StaticFileHandler           static_handler;


public:
    HttpResponseBuilder(const ServerConfig& config);
    ~HttpResponseBuilder();

    HttpResponse buildResponse(Client& client);

    HttpResponse handleGet(const HttpRequest& request, const Location& location);
};

#endif

