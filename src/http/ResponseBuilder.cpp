#include "CgiHandler.hpp"
# include "Request.hpp"
# include "ResponseBuilder.hpp"
# include "Style.hpp"
#include <cstddef>
#include <ostream>
#include <string>

ResponseBuilder::ResponseBuilder(const ServerConfig& config):
    error_handler (config) {
}

ResponseBuilder::~ResponseBuilder() {
}

std::string ResponseBuilder::generateDirectoryListing(const std::string& path) const {
    std::stringstream ss;
    std::vector<std::string> entries = static_handler.listDirectory(path);

    ss << "<!DOCTYPE html><html><head>";
    ss << "<title>Index of " << path << "</title>";
    ss << STYLE;
    ss << "</head><body>";
    ss << "<div class=c>";
    ss << "<h1>Index of " << path << "</h1><hr>";
    ss << "<div class=l>";
    ss << "<div class=i><a href=../>../</a></div>";

    for (size_t i = 0; i < entries.size(); i++) {
        ss << "<div class=i><a href=" << entries[i] << ">";
        ss << entries[i] << "</a></div>";
    }

    ss << "</div></div></body></html>";

    return ss.str();
}

bool ResponseBuilder::isMethodAllowed(const std::string& method, const Location& location) const {
    const std::vector<std::string>& allowed = location.getMethods();
    
    if (allowed.empty()) {
        // If no methods specified, allow GET by default
        return (method == "GET");
    }
    
    for (size_t i = 0; i < allowed.size(); ++i) {
        if (allowed[i] == method) {
            return true;
        }
    }
    
    return false;
}

Response ResponseBuilder::handleAutoIndex(const std::string& path) const {
    Response response;
    
    std::string listing = generateDirectoryListing(path);
    response.setStatusCode(200);
    response.setContentType("text/html");
    response.writeStringToBuffer(listing);
    
    return response;
}

bool ResponseBuilder::isCgiRequest(const std::string& path, const Location& location) const {
    size_t dot_pos = path.find_last_of(".");
    if (dot_pos == std::string::npos) {
        return false;
    }

    std::string extension = path.substr(dot_pos);

    const std::map<std::string, std::string>& cgi_map = location.getCgi();

    return cgi_map.find(extension) != cgi_map.end();
}

Response ResponseBuilder::buildResponse(Request& request) {
    try {
        // check for redirection
        if (request.location.getReturn().first != 0) {
            return handleRedirect(request.location.getReturn().first, request.location.getReturn().second);
        }

        // Check if it's a CGI request
        if (isCgiRequest(request.path, request.location)) {
            return handleCgi (request);
            // handle CGI
// NOTE: test block
std::cout << "Inside isGgiRequest: if this line is reach the request is an executable" << std::endl;
Response response;
response.setStatusCode(900);
response.setContentType("text/html");
response.writeStringToBuffer("<body>Inside Cgi</body>");
return response;
// NOTE: test block end
        }


        // route to handlers
        if (request.method == "GET") {
            return handleGet(request, request.location);
        } else if (request.method == "DELETE") {
            return handleDelete(request.path);
        }
        
        // should never reach here
        Response response;
        response.setStatusCode(500);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(500));
        return response;

    } catch ( const std::exception &e ) {
        Response response;
        response.setStatusCode(500);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(500));
        return response;
    }
}

Response    ResponseBuilder::handleRedirect(int status_code, const std::string& url) const {
    Response response;
    response.setStatusCode(status_code);
    response.setContentType("text/html");
    response.setLocation(url);

    std::stringstream body;
    body << "<!DOCTYPE html><html><head>";
    body << "<title>Redirect</title>";
    body << STYLE;
    body << "</head><body>";
    body << "<div class=e>";
    body << "<h1 style=\"font-size:3rem\">Redirecting...</h1>";
    body << "<hr>";
    body << "<p>You are being redirected to: <br>" << url << "</p>";
    body << "</div></body></html>";

    response.writeStringToBuffer(body.str());

    return response;
}

Response ResponseBuilder::handleGet(const Request& request, const Location& location) {
    Response response;
    const std::string full_path = request.path;

    // Check if path exists
    if (! static_handler.fileExists(full_path)) {
        response.setStatusCode(404);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(404));
        return response;
    }

    // Check if it's a directory
    if (static_handler.isDirectory(full_path)) {
        // Try to serve index file if configured
        if (!location.getIndex().empty()) {
            std::string index_path = full_path;
            if (index_path[index_path.length() - 1] != '/') {
                index_path += "/";
            }
            index_path += location.getIndex();
            if (static_handler.fileExists(index_path) && !static_handler.isDirectory(index_path)) {
                response.setStatusCode(200);
                response.setContentType(static_handler.getContentType(index_path));
                response.writeFileToBuffer(index_path);
                return response;
            }
        }

        // Check if auto_index is enabled
        if (location.getAutoIndex()) {
            //TODO: AutoIndex method
            response = handleAutoIndex(full_path);
            return response;
        } else {
            response.setStatusCode(403);
            response.setContentType("text/html");
            response.writeStringToBuffer(error_handler.generateErrorResponse(403));
            return response;
        }
    }

    // Check if not readable
    if (! static_handler.isReadable(full_path)) {
        response.setStatusCode(403);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(403));
        return response;
    }

    response.setStatusCode(200);
    response.setContentType(static_handler.getContentType(full_path));
    response.writeFileToBuffer(full_path);

    return response;
}

Response ResponseBuilder::handleDelete(std::string full_path) {
    Response response;

    // Check if path exists
    if (! static_handler.fileExists(full_path)) {
        response.setStatusCode(404);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(404));
        return response;
    }

    // if Dir == forbidden
    if (static_handler.isDirectory(full_path)) {
        response.setStatusCode(403);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(403));
        return response;
    }

    if (static_handler.deleteFile(full_path)) {
        response.setStatusCode(204);  // No Content
        response.setContentType("text/html");
        response.setContentLength(0);
    } else {
        response.setStatusCode(500);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(500));
    }

    return response;
}

Response ResponseBuilder::handleCgi(Request& request) {
    CgiHandler cgi_handler(request, request.location);
    return cgi_handler.execute();
}

