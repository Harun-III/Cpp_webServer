#include "CgiHandler.hpp"
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
    // Fork and execute CGI

response.setStatusCode(999);
response.writeStringToBuffer("<body><h1>handle CGI block</h1></doby>");
return response;
}


