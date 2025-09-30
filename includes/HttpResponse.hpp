#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
private:
    int                                 status_code;
    std::string                         status_message;
    std::map<std::string, std::string>  headers;
    std::string                         body;
    std::string                         http_version;

public:
    HttpResponse();
    ~HttpResponse();
};

#endif

