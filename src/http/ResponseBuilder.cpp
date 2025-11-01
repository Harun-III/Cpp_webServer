#include "CgiHandler.hpp"
# include "Core.hpp"
# include "Request.hpp"
# include "ResponseBuilder.hpp"
# include "Style.hpp"
# include "SessionManager.hpp"

ResponseBuilder::ResponseBuilder(const ServerConfig& config):
	error_handler (config) { }

ResponseBuilder::~ResponseBuilder() { }

void ResponseBuilder::handleAutoIndex(const std::string& path, Response& response) const {
	std::string listing = generateDirectoryListing(path);
	response.setStatusCode(200);
	response.setContentType("text/html");
	response.writeStringToBuffer(listing);
}

void ResponseBuilder::handlePost(Response& response) {
	response.setStatusCode(201);
	response.setContentLength(0);
	response.setContentType("text/html");
	response.generateHead();
}

void ResponseBuilder::handleCgi(Request& request, Response& response) {
    switch (response.cgiHandler.getStatus()) {
    case CGI_INIT:
        response.cgiHandler.execute(request);
        // State is set inside execute()
	/* fall through */

    case CGI_PROSSESS:
        response.cgiHandler.checkProcess(response);
	/* fall through */

    case CGI_END:
        response.cgiHandler.processOutput(response);
    }
}

void ResponseBuilder::buildResponse(Request& request, Response& response) {
	// Parse session cookie from request
	request.parseSessionCookie();

	// Set session cookie in response
	response.setCookie("session_id", request.session_id);

	// Handle special session API endpoints
	if (request.target == "/api/session") {
		handleSessionApi(request, response);
		return;
	}

	if (request.detectRoute == RT_REDIR) {
		handleRedirect(request, response);
	}

	else if (request.detectRoute == RT_CGI) {
		handleCgi(request, response);
	}

	else if (request.method == "GET") {
		handleGet(request, request.location, response);
	}

	else if (request.method == "DELETE") {
		handleDelete(request.path, response);
	}

	else if (request.method == "POST") {
		handlePost(response);
	}
}

void ResponseBuilder::handleSessionApi(Request& request, Response& response) {
    SessionManager& sm = SessionManager::getInstance();
    SessionData& session = sm.getSession(request.session_id);
    
    if (request.method != "GET") {
        throw State(405, BAD);
    }
    
    // Check if this is an update request (has query string)
    if (!request.query.empty()) {
        // Parse query string: ?set=theme&value=dark
        std::string set_param;
        std::string value_param;
        
        // Query parser
        size_t set_pos = request.query.find("set=");
        if (set_pos != std::string::npos) {
            size_t start = set_pos + 4;
            size_t end = request.query.find('&', start);
            if (end == std::string::npos) end = request.query.length();
            set_param = request.query.substr(start, end - start);
        }
        
        size_t value_pos = request.query.find("value=");
        if (value_pos != std::string::npos) {
            size_t start = value_pos + 6;
            size_t end = request.query.find('&', start);
            if (end == std::string::npos) end = request.query.length();
            value_param = request.query.substr(start, end - start);
        }
        
        // Update session based on parameters
        if (set_param == "theme") {
            session.theme = value_param;
            sm.updateSession(request.session_id, session);
        } else if (set_param == "language") {
            session.language = value_param;
            sm.updateSession(request.session_id, session);
        }
    }
    
    // Return current session data as JSON
    std::stringstream json;
    json << "{";
    json << "\"session_id\":\"" << request.session_id << "\",";
    json << "\"theme\":\"" << session.theme << "\",";
    json << "\"language\":\"" << session.language << "\"";
    json << "}";

    response.setStatusCode(200);
    response.setContentType("application/json");
    response.writeStringToBuffer(json.str());
    throw State(0, WRITING);
}

void    ResponseBuilder::handleRedirect(Request& request , Response& response) const {
	int                 status_code = request.location.getReturn().first;
	const std::string&  url = request.location.getReturn().second;

	// Prepare response
	response.setStatusCode(status_code);
	response.setContentType("text/html");
	response.setLocation(url);

	// Generate redirect body 
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

	// Write to response
	response.writeStringToBuffer(body.str()); throw State(0, WRITING);
}

std::string ResponseBuilder::generateDirectoryListing(const std::string& path) const {
	std::stringstream ss;
	std::vector<std::string> entries = static_handler.listDirectory(path);

	// Generate HTML
	ss << "<!DOCTYPE html><html><head>";
	ss << "<title>Index of " << path << "</title>";
	ss << STYLE << DELETE_SCRIPT;
	ss << "</head><body>";
	ss << "<div class=c>";
	ss << "<h1>Index of " << path << "</h1><hr>";
	ss << "<div class=l>";
	ss << "<div class=i><a class=parent href=../>../</a></div>";

	// List entries
	for (size_t i = 0; i < entries.size(); i++) {
		bool    isDir = (entries[i][entries[i].length() - 1] == '/');
		ss << "<div class=i><a href=" << entries[i] << ">";
		ss << entries[i] << "</a>";

		// Add delete button for files only
		if (!isDir) ss << "<button class=d onclick=\"showModal('"
			<< entries[i] << "',this)\">DEL</button>";

		ss << "</div>";
	}

	ss << "</div></div></body></html>";
	return ss.str();
}

void ResponseBuilder::handleDelete(const std::string& full_path, Response& response) {
	// Check if path exists
	if (!static_handler.fileExists(full_path)) {
		throw State(404, BAD);
	}

	// if Dir == forbidden
	if (static_handler.isDirectory(full_path)) {
		throw State(403, BAD);
	}

	// Attempt to delete file	
	if (static_handler.deleteFile(full_path)) {
		response.setStatusCode(204);
		response.setContentType("text/html");
		response.setContentLength(0);
		response.generateHead();
	}

	else throw State(500, BAD);
}

void ResponseBuilder::handleGet(const Request& request, const Location& location, Response& response) {
	std::string				target = request.path;

	// Check if path exists
	if (!static_handler.fileExists(target)) {
		throw State(404, BAD);
	}

	// Check if it's a directory
	if (static_handler.isDirectory(target)) {
		const std::string	index_path = request.joinPath(target, location.getIndex());

		// Check for index file
		if (!location.getIndex().empty() && static_handler.fileExists(index_path)
				&& !static_handler.isDirectory(index_path)) {
			target = index_path;
		}

		// Handle autoindex
		else if (location.getAutoIndex()) {
			handleAutoIndex(target, response); return;
		}

		// No index file and autoindex disabled
		else throw State(403, BAD);
	}

	// Check if not readable
	if (!static_handler.isReadable(target)) {
		throw State(403, BAD);
	}

	// Get file size
	struct stat				statusBuffer;
	if (stat(target.c_str(), &statusBuffer) == -1) throw State(404, BAD);

	// Prepare response
	response.setStatusCode(200);
	response.setContentType(static_handler.getContentType(target));
	response.setContentLength(statusBuffer.st_size);
	response.generateHead();

	// Open file stream
	response.bodyStream.open(target.c_str(), std::ios::in | std::ios::binary);
	if (!response.bodyStream.is_open()) throw State(500, BAD);

	throw State(0, WRITING);
}
