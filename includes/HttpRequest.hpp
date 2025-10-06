#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
private:

public:
    std::string                         method;
    std::string                         full_path;
    std::map<std::string, std::string>  headers;
    std::string                         body;
};

#endif
