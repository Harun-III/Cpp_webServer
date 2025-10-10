#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

class HttpResponse {
private:
    std::string                         http_version;
    int                                 status_code;
    std::string                         status_message;
    std::map<std::string, std::string>  headers;

    //NOTE: This string is here for testing purposes in practise I'll write directly 
    // to the buffer
    std::string                         body;

public:
    HttpResponse();
    ~HttpResponse();

    void setStatusCode(int code);
    void setStatusMessage(const std::string& message);
    void setContentType(const std::string& type);
    void setHeader(const std::string& name, const std::string& value);
    void setLocation(const std::string& url);

    // Getters
    int                     getStatusCode() const;
    const std::string&      getStatusMessage() const;

    // Build the HTTP response string
    std::string toString() const;

    static      std::string getStatusText(int code);
    void        setContentLength(size_t length);
    void        writeFileToBuffer(std::string full_path);
    // This now also sets the "Content-Lenght"
    void        writeStringToBuffer(std::string str);
};

#endif

