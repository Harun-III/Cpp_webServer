#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "Core.hpp"
# include "Request.hpp"
# include "ErrorHandler.hpp"
# include "CgiHandler.hpp"

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

    std::string             generated;
    std::ifstream           bodyStream;
	CgiHandler              cgiHandler;
	std::string             cgiFilePath;
};

#endif
