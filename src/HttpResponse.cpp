// #include "HttpResponse.hpp"

// delte this later
#include "../includes/HttpResponse.hpp"

HttpResponse::HttpResponse() : http_version("HTTP/1.0") {
}

HttpResponse::~HttpResponse() {
}

void HttpResponse::setStatusCode(int code) {
    status_code = code;
    status_message = getStatusText(code);
}

void HttpResponse::setStatusMessage(const std::string& message) {
    status_message = message;
}

void HttpResponse::setContentType(const std::string& type) {
    setHeader("Content-Type", type);
}

void HttpResponse::setHeader(const std::string& name, const std::string& value) {
    headers[name] = value;
}

void HttpResponse::setLocation(const std::string& url) {
    setHeader("Location", url);
}

#include <iostream>
std::string HttpResponse::toString() const {
    std::stringstream ss;
    
    // Status line
    ss << http_version << " " << status_code << " " << status_message << "\r\n";
    
    // Headers
    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it) {
        ss << it->first << ": " << it->second << "\r\n";
    }

    // Empty line between headers and body
    ss << "\r\n";
    // Body
    ss << body;

    return ss.str();
}

std::string HttpResponse::getStatusText(int code) {
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
        
        // 5xx Server Error
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 505: return "HTTP Version Not Supported";
        
        default: return "Unknown";
    }
}

void HttpResponse::setContentLength(size_t length) {
    std::stringstream ss;
    ss << length;
    setHeader("Content-Length", ss.str());
}

void HttpResponse::writeStringToBuffer(std::string str) {
    body = str;
    setContentLength(body.length());
}

void HttpResponse::writeFileToBuffer(std::string full_path) {
    struct stat s_buffer;
    if (stat(full_path.c_str(), &s_buffer) != 0) {
        setContentLength(0);
        body = "";
        return;
    }

    // Set content length before reading
    setContentLength(s_buffer.st_size);

    std::ifstream file(full_path.c_str());
    if (!file.is_open()) {
        setContentLength(0);
        body = "";
        return;
    }

    // Reserve space in the string to avoid reallocations
    body.resize(s_buffer.st_size);

    file.read(&body[0], s_buffer.st_size);
    file.close();
}

