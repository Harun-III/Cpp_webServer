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

public:
    Response();
    ~Response();

	size_t					cgi_offset;

    void					setStatusCode(int code);
    void					setStatusMessage(const std::string& message);
    void					setContentType(const std::string& type);
    void					setHeader(const std::string& name, const std::string& value);
    void					setLocation(const std::string& url);
    void                    setContentLength(size_t length);
    
    // Getters
    int                     getStatusCode() const;
    const std::string&      getStatusMessage() const;
    std::string             to_string(int code);

    void                    generateErrorPage(const ServerConfig &server, int code);
    static                  std::string getStatusText(int code);
    bool                    continueStreaming( void );

    // This now also sets the "Content-Lenght"
    void                    generateHead();
    void                    writeStringToBuffer(std::string str);

	std::ifstream			file;
    std::string             generated;
    std::fstream            bodyStream;
};

#endif
