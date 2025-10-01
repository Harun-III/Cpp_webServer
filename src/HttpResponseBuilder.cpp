#include "../includes/HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder(const ServerConfig& config):
    server_config (config) {
}

HttpResponseBuilder::~HttpResponseBuilder() {
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
full_path = "./test_files/path_test_in_get";
/**********************************/

/*
//NOTE:
    Check if path exists
        if not 404
    Check if it's a directory
        if index --> serve index
        if auto_index --> list files in dir
    else return error page
*/

    // Check if path exists
    if (! static_handler.fileExists(full_path)) {
        response.setStatusCode(404);
        return response;
    }


    
   
/******** //NOTE: TESTING *********/
    response.setStatusCode(900);
    return response;
/**********************************/
}

