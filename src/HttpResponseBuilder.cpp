// #include "HttpResponseBuilder.hpp"

// NOTE: Delte this later
#include "../includes/HttpResponseBuilder.hpp"
#include "../src/http/Request.hpp"

HttpResponseBuilder::HttpResponseBuilder(const ServerConfig& config):
    server_config (config), error_handler (config) {
}

HttpResponseBuilder::~HttpResponseBuilder() {
}

std::string HttpResponseBuilder::generateDirectoryListing(const std::string& path) const {
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

bool HttpResponseBuilder::isMethodAllowed(const std::string& method, const Location& location) const {
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

HttpResponse HttpResponseBuilder::handleAutoIndex(const std::string& path) const {
    HttpResponse response;
    
    std::string listing = generateDirectoryListing(path);
    response.setStatusCode(200);
    response.setContentType("text/html");
    response.writeStringToBuffer(listing);
    
    return response;
}

HttpResponse HttpResponseBuilder::buildResponse(Request& request) {
    // check if the method is implemented
    if (request.method != "GET" && request.method != "POST" && request.method != "DELETE") {
        HttpResponse response;
        response.setStatusCode(501);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(501));
        return response;
    }

    // check if METHOD is allowed in location
    if (!isMethodAllowed(request.method, request.location)) {
        HttpResponse response;
        response.setStatusCode(405);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(405));
        return response;
    }

// NOTE: test block redirection -------------
std::string full_path = "https://www.google.com";
request.location.setReturn(302, full_path);
//

    // check for redirection
    if (request.location.getReturn().first != 0) {
        return handleRedirect(request.location.getReturn().first, request.location.getReturn().second);
    }

    // route to handlers
    if (request.method == "GET") {
        return handleGet(request, request.location);
    } else if (request.method == "DELETE") {
        // return handleDelete(Connection.request, Connection.location);
    } else if (request.method == "POST") {
        // return handlePost(Connection.request, Connection.location);
    }

//NOTE: these lines are here just to suppress the warning from the compiler
// and it will work as default response
    HttpResponse response;
    response.setStatusCode(200);
    return response;
}

HttpResponse    HttpResponseBuilder::handleRedirect(int status_code, const std::string& url) const {
    HttpResponse response;
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

HttpResponse HttpResponseBuilder::handleGet(const Request& request, const Location& location) {
    HttpResponse response;
    std::string full_path = request.full_path;

/******** //NOTE: TESTING *********/
// full_path = "./test_files/regular_readable_file";
full_path = "./test_files/no_permissions";
// full_path = "./test_files/file.txt";
// full_path = "./test_files/no_exist";
// full_path = "./test_files";
// full_path = "./test_files/";
// full_path = "./test_files/index.html";
// full_path = "./";
// full_path = "/home/";
/**********************************/

/*
//NOTE:
    Check if path exists
        if not 404
    Check if it's a directory
        if index --> serve index
        if auto_index --> list files in dir
    Check if ! readable
        403 "Forbidden"
    server regular file
*/

    // Check if path exists
    if (! static_handler.fileExists(full_path)) {
        response.setStatusCode(404);
        response.setContentType("text/html");
        response.writeStringToBuffer(error_handler.generateErrorResponse(404));
        return response;
    }

    // Check if it's a directory
    if (static_handler.isDirectory(full_path)) {
        /******** //NOTE: TESTING *********/
            std::cout << "It'a a direcory" << std::endl;
        /**********************************/
        // Try to serve index file if configured
        if (!location.getIndex().empty()) {
            std::string index_path = full_path;
            if (index_path[index_path.length() - 1] != '/') {
                index_path += "/";
            }
            /******** //NOTE: TESTING *********/
            std::cout << "index_path is: " << index_path << std::endl;
            /**********************************/
            index_path += location.getIndex();
            /******** //NOTE: TESTING *********/
            std::cout << "index_path + index is: " << index_path << std::endl;
            /**********************************/
            if (static_handler.fileExists(index_path) && !static_handler.isDirectory(index_path)) {
                /******** //NOTE: TESTING *********/
                std::cout << "append index.html to file\n\n";
                /**********************************/
                response.setStatusCode(200);
                response.setContentType(static_handler.getContentType(index_path));
                response.writeFileToBuffer(index_path);
                return response;
            }
        }
        // Check if auto_index is enabled
        if (location.getAutoIndex()) {
            /******** //NOTE: TESTING *********/
            std::cout << "AutoIndex is on\n";
            /**********************************/
            //TODO: AutoIndex method
            response = handleAutoIndex(full_path);
            return response;
        } else {
            /******** //NOTE: TESTING *********/
            std::cout << "AutoIndex is off, returning page 403, Forbidden\n";
            /**********************************/
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

    /******** //NOTE: TESTING *********/
    // std::cout << "file is readable returning a 200 OK page\n\n";
    /**********************************/

    response.setStatusCode(200);
    response.setContentType(static_handler.getContentType(full_path));
    // this will write to the response.body string at the moment
    response.writeFileToBuffer(full_path);

    return response;
}

