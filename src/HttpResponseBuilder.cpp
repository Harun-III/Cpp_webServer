#include "../includes/HttpResponseBuilder.hpp"

HttpResponseBuilder::HttpResponseBuilder(const ServerConfig& config):
    server_config (config) {
}

HttpResponseBuilder::~HttpResponseBuilder() {
}

HttpResponse HttpResponseBuilder::buildResponse(const HttpRequestData& request, Client& client) {

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

    if (request.method == "GET") {
        //TODO: function that handles get
        return handleGet(request, client.location);
    }


//NOTE: these lines are here just to supress the worning from the compiler
    HttpResponse response;
    response.setStatusCode(200);
    return response;
}

HttpResponse HttpResponseBuilder::handleGet(const HttpRequestData& request, const Location& location) {
    std::string full_path = request.full_path;
   
/******** //NOTE: TESTING *********/
    HttpResponse response;
    response.setStatusCode(200);
    return response;
/**********************************/
}

