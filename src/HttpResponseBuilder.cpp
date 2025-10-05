#include "../includes/HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder(const ServerConfig& config):
    server_config (config) {
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

HttpResponse HttpResponseBuilder::handleAutoIndex(const std::string& path) const {
    HttpResponse response;
    
    std::string listing = generateDirectoryListing(path);
    response.setStatusCode(200);
    response.setContentType("text/html");
    response.writeStringToBuffer(listing);
    
    return response;
}


HttpResponse HttpResponseBuilder::buildResponse(Client& client) {

/*
    HttpRequestData request;
        request.method = "GET";
        request.path = "/";
        request.headers["head1"] = "head_one";

    ServerConfig& server_config;
        listen;
        locations;
        error_pages;
        max_client_body_size;

    GET / http/1.1
    head1: head_one

    This is the body
*/

/*
    1_ check if the method is implemented
    2_ if METHOD == POST
        check for body_size
    3_ find location
    4_ check if METHOD is allowed in location
    5_ route to handlers
*/

    // find location
    //Location location = server_config.getLocations()

    if (client.request.method == "GET") {
        //TODO: function that handles get
        return handleGet(client.request, client.location);
    }


//NOTE: these lines are here just to suppress the warning from the compiler
// and it will work as default response
    HttpResponse response;
    response.setStatusCode(200);
    return response;
}

HttpResponse HttpResponseBuilder::handleGet(const HttpRequest& request, const Location& location) {
    HttpResponse response;
    std::string full_path = request.full_path;

/******** //NOTE: TESTING *********/
// full_path = "./test_files/regular_readable_file";
// full_path = "./test_files/no_permissions";
// full_path = "./test_files/file.txt";
// full_path = "./test_files/no_exist";
// full_path = "./test_files";
// full_path = "./test_files/";
// full_path = "./test_files/index.html";
// full_path = "./";
full_path = "/home/";
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
        response.writeFileToBuffer("./errors/404.html");
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
            response.writeFileToBuffer("./errors/403.html");
            return response;
        }

    }

    // Check if not readable
    if (! static_handler.isReadable(full_path)) {
        response.setStatusCode(403);
        response.setContentType("text/html");
        response.writeFileToBuffer("./errors/403.html");
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

