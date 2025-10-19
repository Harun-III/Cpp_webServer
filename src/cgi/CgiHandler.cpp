#include "CgiHandler.hpp"
#include <cstring>
#include <string>
#include <unistd.h>

CgiHandler::CgiHandler(const Request& req, const Location& loc)
    : request(req), location(loc), script_path(""), cgi_executable("") {
}

CgiHandler::~CgiHandler() {
}

std::string CgiHandler::getFileExtension(const std::string& path) const {
	size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "";
    }
    return path.substr(dot_pos);
}

std::string CgiHandler::getCgiExecutable(const std::string& file_path) const{
	std::string extension = getFileExtension(file_path);
    const std::map<std::string, std::string>& cgi_map = location.getCgi();

    std::map<std::string, std::string>::const_iterator it = cgi_map.find(extension);
    if (it != cgi_map.end()) {
        return it->second;
    }
    return "";
}

Response CgiHandler::execute() {
    Response response;

    getFileExtension(request.path);
    // Set script path and CGI executable
    script_path = request.path;
    cgi_executable = getCgiExecutable(script_path);

    // Check if script exists and is executable
    std::cout << "Script to check: " << script_path.c_str() << std::endl;
    if (access(script_path.c_str(), F_OK) != 0) {
        response.generateErrorPage(request.server, 404);
        return response;
    }
    
    if (access(script_path.c_str(), X_OK) != 0) {
        response.generateErrorPage(request.server, 403);
        return response;
    }

    // Create pipes for communication
    int pipe_in[2]; // send to CGI
    int pipe_out[2];  // receiving from CGI
    
    if (pipe(pipe_out) < 0) {
        response.generateErrorPage(request.server, 500);
        return response;
    }

    // For POST, use file instead of pipe for input
    // NOTE: chane this line when done testing
    // if (request.method == "POST") {
    if (0) {
	// generate temp file name for input_file
	// put str request.recv into file
	// if anything fails == 500
    } else if (pipe(pipe_in) < 0) {
	close(pipe_out[0]);
	close(pipe_out[1]);
	response.generateErrorPage(request.server, 500);
	return response;
    }

    // Build environment variables and arguments
    char** env = buildEnvVariables();
    
/////// TEST: print env
for (int i = 0; env && env[i]; i++) {
    std::cout << env[i] << std::endl;
}
/////// TEST: end

    // Fork and execute CGI

response.setStatusCode(999);
response.writeStringToBuffer("<body><h1>handle CGI block</h1></doby>");
return response;
}

char** CgiHandler::buildEnvVariables () const {
    std::vector<std::string> env_strings;

    env_strings.push_back("REQUEST_METHOD=" + request.method);
    env_strings.push_back("SCRIPT_FILENAME=" + script_path);
    env_strings.push_back("SCRIPT_NAME=" + request.target);
    env_strings.push_back("QUERY_STRING=" + request.query);
    env_strings.push_back("SERVER_PROTOCOL=" + request.version);
    env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");

    const std::pair<std::string, std::string>& listen = request.server.getListen()[0];
    env_strings.push_back("SERVER_NAME=" + listen.first);
    env_strings.push_back("SERVER_PORT=" + listen.second);

    // If method is POST get set CONTENT_LENGTH
    // check if request.has_content_length
    // set --> CONTENT_LENGTH= + request.content_length

    // Content-Type from headers
    map_t::const_iterator content_type = request.headers.find("content-type");
    if (content_type != request.headers.end()) {
        env_strings.push_back("CONTENT_TYPE=" + content_type->second);
    }

    // HTTP headers - these need to be convert to HTTP_* format
    // 4.1.18.  Protocol-Specific Meta-Variables (RFC 3785)

    char** env = new char*[env_strings.size() + 1];
    for (size_t i = 0; i < env_strings.size(); ++i) {
        env[i] = new char[env_strings[i].length() + 1];
        std::strcpy(env[i], env_strings[i].c_str());
    }
    env[env_strings.size()] = NULL;
    
    return env;
}

