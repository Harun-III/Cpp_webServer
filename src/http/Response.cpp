# include "Response.hpp"
#include "Request.hpp"

Response::Response() : http_version("HTTP/1.0") { }

Response::~Response() {
}

std::string Response::to_string(int code) {
	std::stringstream   ss;
	return (ss << code, ss.str());
}

void Response::setStatusCode(int code) {
	status_code = code;
	status_message = getStatusText(code);
}

void Response::setStatusMessage(const std::string& message) {
	status_message = message;
}

const std::string&      Response::getStatusMessage() const {
	return status_message;
}

void Response::setContentType(const std::string& type) {
	setHeader("Content-Type", type);
}

void Response::setHeader(const std::string& name, const std::string& value) {
	headers[name] = value;
}

void Response::setLocation(const std::string& url) {
	setHeader("Location", url);
}

void Response::setTotalSize(size_t size) {
	total_size = size;
}

size_t Response::getTotalSize() const {
	return total_size;
}

void Response::setContentLength(size_t length) {
	setHeader("Content-Length", to_string(length));
}

void Response::writeStringToBuffer(std::string str) {
	setContentLength(str.length());
	generateHead(); generated += str;
}

void Response::generateErrorPage(const ServerConfig &server, int code) {
	ErrorHandler    error_handler(server);

	setStatusCode(code);
	setContentType("text/html");
	writeStringToBuffer(error_handler.generateErrorResponse(code));
}

void    Response::generateHead() {
	// Generate Response line
	generated.append(http_version)
			.push_back(' ');
	generated.append(to_string(status_code))
			.push_back(' ');
	generated.append(status_message)
			.append("\r\n");

	// Headers
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		it != headers.end(); ++it) {
		generated.append(it->first)
				.append(": ")
				.append(it->second)
				.append("\r\n");
	}

	// Empty line between headers and body
	generated.append("\r\n");
}

int Response::getStatusCode() const {
	return status_code;
}

std::string Response::getStatusText(int code) {
	switch (code) {
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 204: return "No Content";

		// 3xx Redirection
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";

		// 4xx Client Error
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 408: return "Request Timeout";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 431: return "Request Header Fields Too Large";

		// 5xx Server Error
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 505: return "HTTP Version Not Supported";

		default: return "Unknown";
	}
}

bool Response::continueStreaming(void) {
	std::cout << "continueStreaming ..." << std::endl;

	size_t		have = generated.size();
	if (have >= BUF_SIZE) return false;

	if (!bodyStream.is_open()) return generated.empty();

	size_t		need = BUF_SIZE - have;
	char		buf[BUF_SIZE];

	bodyStream.read(buf, static_cast<std::streamsize>(need));
	std::streamsize		len = bodyStream.gcount();

	if (len > 0) { generated.append(buf, static_cast<size_t>(len)); }

	if (bodyStream.eof()) bodyStream.close();
	return (!bodyStream.is_open() && generated.empty());
}
