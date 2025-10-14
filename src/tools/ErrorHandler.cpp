#include "ErrorHandler.hpp"
#include "ResponseBuilder.hpp"
#include <sstream>

ErrorHandler::ErrorHandler(const ServerConfig& config) : server_config(config) {
}

ErrorHandler::~ErrorHandler() {
}

std::string ErrorHandler::getErrorPagePath(int error_code) const {
    const std::map<int, std::string>& error_pages = server_config.getErrorPages();

    std::map<int, std::string>::const_iterator it = error_pages.find(error_code);
    if (it != error_pages.end()) {
	return it->second;
    }

    return "";
}

std::string ErrorHandler::generateErrorResponse(int error_code) const {
    if (hasCustomErrorPage(error_code)) {
	std::string error_page_path = getErrorPagePath(error_code);

	StaticFileHandler static_handler;
	if (static_handler.fileExists(error_page_path)
		&& static_handler.isReadable(error_page_path)) {
	    return static_handler.readFile(error_page_path);
	}
    }

    return generateDefaultErrorPage(error_code);
}

bool ErrorHandler::hasCustomErrorPage(int error_code) const {
    const std::map<int, std::string> error_pages = server_config.getErrorPages();
    return error_pages.find(error_code) != error_pages.end();
}

std::string ErrorHandler::generateDefaultErrorPage(int error_code) const {
    Response Response;
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
