#include "CgiHandler.hpp"
# include "Request.hpp"
# include "ResponseBuilder.hpp"
# include "Style.hpp"
#include <cstddef>
#include <ostream>
#include <string>

ResponseBuilder::ResponseBuilder(const ServerConfig& config):
	error_handler (config) { }

ResponseBuilder::~ResponseBuilder() { }

void ResponseBuilder::handleCgi(Request& request, Response& response) {
	CgiHandler cgi_handler(request, request.location);
	cgi_handler.execute(response);
}

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

void ResponseBuilder::buildResponse(Request& request, Response& response) {
	if (request.detectRoute == REDIR) {
		handleRedirect(request, response);
	}

	if (request.detectRoute == CGI) {
		handleCgi(request, response);
	}

	if (request.method == "GET") {
		handleGet(request, request.location, response);
	}

	if (request.method == "DELETE") {
		handleDelete(request.path, response);
	}

	if (request.method == "POST") {
		handlePost(response);
	}
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
