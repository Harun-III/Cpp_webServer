#ifndef HTTPRESPONSEBUILDER_HPP
#define HTTPRESPONSEBUILDER_HPP

#include "HttpResponse.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/Client.hpp"
#include <string>
#include <map>

struct HttpRequestData {
    std::string                         method;
    std::string                         full_path;
    std::map<std::string, std::string>  headers;
    std::string                         body;
};

class HttpResponseBuilder {
private:
    const ServerConfig& server_config;

public:
    HttpResponseBuilder(const ServerConfig& config);
    ~HttpResponseBuilder();

    HttpResponse buildResponse(const HttpRequestData& request, Client& client);

    HttpResponse handleGet(const HttpRequestData& request, const Location& location);
};

#endif

