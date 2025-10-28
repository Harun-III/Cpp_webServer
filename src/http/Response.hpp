#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "Core.hpp"
# include "ErrorHandler.hpp"
// # include "ServerConfig.hpp"

class ServerConfig;

class Response {
private:
    std::string                         http_version;
    int                                 status_code;
    std::string                         status_message;
    std::map<std::string, std::string>  headers;

    size_t                              total_size;

public:
    Response();
    ~Response();

    void setTotalSize(size_t code);
    void setStatusCode(int code);
    void setStatusMessage(const std::string& message);
    void setContentType(const std::string& type);
    void setHeader(const std::string& name, const std::string& value);
    void setLocation(const std::string& url);
    
    // Getters
    int                     getStatusCode() const;
    const std::string&      getStatusMessage() const;
    size_t                  getTotalSize() const;
    std::string             to_string(int code);

    void                    generateErrorPage(const ServerConfig &server, int code);
    static                  std::string getStatusText(int code);
    void                    setContentLength(size_t length);
    bool                    continueStreaming( void );

    // This now also sets the "Content-Lenght"
    void                    generateHead();
    void                    writeStringToBuffer(std::string str);

    std::string             generated;
    std::fstream            bodyStream;
};

#endif
