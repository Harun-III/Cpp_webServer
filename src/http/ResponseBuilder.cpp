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
    ss << STYLE << DELETE_SCRIPT;
    ss << "</head><body>";
    ss << "<div class=c>";
    ss << "<h1>Index of " << path << "</h1><hr>";
    ss << "<div class=l>";
    ss << "<div class=i><a class=parent href=../>../</a></div>";

    for (size_t i = 0; i < entries.size(); i++) {
        bool isDir = (entries[i][entries[i].length() - 1] == '/');
        ss << "<div class=i><a href=" << entries[i] << ">";
        ss << entries[i] << "</a>";

        if (!isDir) ss << "<button class=d onclick=\"showModal('"
            << entries[i] << "',this)\">DEL</button>";

        ss << "</div>";
    }

    ss << "</div></div></body></html>";

    return ss.str();
}

void ResponseBuilder::handleAutoIndex(const std::string& path, Response& response) const {
    std::string listing = generateDirectoryListing(path);
    response.setStatusCode(200);
    response.setContentType("text/html");
    response.writeStringToBuffer(listing);
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

void ResponseBuilder::buildResponse(Request& request, Response& response) {
    try {
        // check for redirection
        if (request.location.getReturn().first != 0) {
            handleRedirect(request.location.getReturn().first, request.location.getReturn().second, response);
            return;
        }

        // Check if it's a CGI request
        if (isCgiRequest(request.path, request.location)) {
            handleCgi(request, response);
            return;
        }

        // route to handlers
        if (request.method == "GET") {
            handleGet(request, request.location, response);
            return;
        } else if (request.method == "DELETE") {
            handleDelete(request.path, response);
            return;
        } else if (request.method == "POST") {
            response.setStatusCode(201);
            response.setContentLength(0);
            return;
        }

        // should never reach here
        response.setStatusCode(500);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(500));

    } catch (const std::exception &e) {
        response.setStatusCode(500);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(500));
    }
}

void ResponseBuilder::handleRedirect(int status_code, const std::string& url, Response& response) const {
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
}

void ResponseBuilder::handleGet(const Request& request, const Location& location, Response& response) {
    const std::string full_path = request.path;

    // Check if path exists
    if (!static_handler.fileExists(full_path)) {
        response.setStatusCode(404);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(404));
        return;
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
                return;
            }
        }

        // Check if auto_index is enabled
        if (location.getAutoIndex()) {
            handleAutoIndex(full_path, response);
            return;
        } else {
            response.setStatusCode(403);
            response.setContentType("text/html");
            response.writeStringToBuffer(error_handler.generateErrorResponse(403));
            return;
        }
    }

    // Check if not readable
    if (!static_handler.isReadable(full_path)) {
        response.setStatusCode(403);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(403));
        return;
    }

    response.setStatusCode(200);
    response.setContentType(static_handler.getContentType(full_path));
    response.writeFileToBuffer(full_path);
}

void ResponseBuilder::handleDelete(const std::string& full_path, Response& response) {
    // Check if path exists
    if (!static_handler.fileExists(full_path)) {
        response.setStatusCode(404);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(404));
        return;
    }

    // if Dir == forbidden
    if (static_handler.isDirectory(full_path)) {
        response.setStatusCode(403);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(403));
        return;
    }

    if (static_handler.deleteFile(full_path)) {
        response.setStatusCode(204);
        response.setContentType("text/html");
        response.setContentLength(0);
    } else {
        response.setStatusCode(500);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(500));
    }
}

void ResponseBuilder::handleCgi(Request& request, Response& response) {
    CgiHandler cgi_handler(request, request.location);
    cgi_handler.execute(response);
}

