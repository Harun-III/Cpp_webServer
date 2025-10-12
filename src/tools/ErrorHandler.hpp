#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include "ServerConfig.hpp"
#include <string>

class ErrorHandler {
private:
    const           ServerConfig& server_config;
    std::string     generateDefaultErrorPage(int error_code) const;

public:
    ErrorHandler(const ServerConfig& config);
    ~ErrorHandler();

    std::string getErrorPagePath(int error_code) const;
    std::string generateErrorResponse(int error_code) const;
    bool        hasCustomErrorPage(int error_code) const;
};

#endif

