# include "Request.hpp"
# include "ResponseBuilder.hpp"

ResponseBuilder::ResponseBuilder(const ServerConfig& config):
    error_handler (config) {
}

ResponseBuilder::~ResponseBuilder() {
}

std::string ResponseBuilder::generateDirectoryListing(const std::string& path) const {
    std::stringstream ss;
    std::vector<std::string> entries = static_handler.listDirectory(path);
    
    ss << "<html><head><title>Index of " << path << "</title></head>\n";
    ss << "<body>\n";
    ss << "<h1>Index of " << path << "</h1>\n";
    ss << "<hr>\n";   

    for (size_t i = 0; i < entries.size(); i++) {
        ss << entries[i] << "<br>\n";
    }
    
    ss << "</body>\n";
    ss << "</html>\n";
    
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

Response ResponseBuilder::buildResponse(Request& request) {
    try {
        // check if METHOD is allowed in location
        if (!isMethodAllowed(request.method, request.location)) {
            Response response;
            response.setStatusCode(405);
            response.setContentType("text/html");
            response.writeStringToBuffer(error_handler.generateErrorResponse(405));
            return response;
        }

        // check for redirection
        if (request.location.getReturn().first != 0) {
            return handleRedirect(request.location.getReturn().first, request.location.getReturn().second);
        }

        // route to handlers
        if (request.method == "GET") {
            return handleGet(request, request.location);
        } else if (request.method == "DELETE") {
            return handleDelete(request.full_path);
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
    body << "<html><head><title>Redirect</title></head>";
    body << "<body><h1>Redirecting...</h1>";
    body << "<p>You are being redirected to: " << url << "</p>";
    body << "</body></html>";
    
    response.writeStringToBuffer(body.str());

    return response;
}

Response ResponseBuilder::handleGet(const Request& request, const Location& location) {
    Response response;
    const std::string full_path = request.full_path;

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
