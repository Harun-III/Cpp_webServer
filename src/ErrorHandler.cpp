#include "../includes/ErrorHandler.hpp"
#include "../includes/HttpResponseBuilder.hpp"
#include <sstream>

ErrorHandler::ErrorHandler(const ServerConfig& config) : server_config(config) {
}

ErrorHandler::~ErrorHandler() {
}

std::string ErrorHandler::generateErrorResponse(int error_code) const {
    // First try to load custom error page
    //if (// has Custom Error Page) {
	//NOTE: use staticFileHandler to check if exist and readable
	//if so return it
	//return ("This a place holder: this should return a costum error page");
    //}
    
    // Generate default error page
    return generateDefaultErrorPage(error_code);
}

std::string ErrorHandler::generateDefaultErrorPage(int error_code) const {
    HttpResponse Response;
    std::string error_message = Response.getStatusText(error_code);

    std::stringstream ss;

    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head>\n";
    ss << "    <title>" << error_code << " " << error_message << "</title>\n";
    ss << "</head>\n";
    ss << "<body>\n";
    ss << "    <h1>" << error_code << "</h1>\n";
    ss << "    <p>" << error_message << "</p>\n";
    ss << "</body>\n";
    ss << "</html>\n";

    return ss.str();
}

