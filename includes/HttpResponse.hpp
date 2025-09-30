#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <sstream>

class HttpResponse {
private:
    int                                 status_code;
    std::string                         status_message;
    std::map<std::string, std::string>  headers;
    std::string                         http_version;

public:
    HttpResponse();
    ~HttpResponse();

    void setStatusCode(int code);
    void setStatusMessage(const std::string& message);
    
    // Getters
    int getStatusCode() const;
    const std::string& getStatusMessage() const;

    // Build the HTTP response string
    std::string toString() const;

    static std::string getStatusText(int code);
};

#endif

