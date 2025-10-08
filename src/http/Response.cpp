// Response.cpp

#include "Core.hpp"
#include <fstream>
#include <iterator>

void	Connection::buildResponseMinimal(void) {
	std::cout << "Client Socket: " << soc << std::endl;

	// --- Load file ./www/Index.html into 'body'
	std::ifstream	file("./www/index.html", std::ios::in | std::ios::binary);
	std::string		body;
	bool			ok = false;

	if (file) {
		file.seekg(0, std::ios::end);
		std::streampos sz = file.tellg();
		if (sz > 0) {
			body.resize((size_t)sz);
			file.seekg(0, std::ios::beg);
			file.read(&body[0], sz);
			ok = true;
		} else {
			file.clear();
			file.seekg(0, std::ios::beg);
			body.assign(std::istreambuf_iterator<char>(file),
						std::istreambuf_iterator<char>());
			ok = true;
		}
	}

	const char* status = ok ? "200 OK" : "404 Not Found";
	if (!ok) {
		body =
			"<!DOCTYPE html>\n"
			"<html><head><title>404 Not Found</title></head>\n"
			"<body><h1>404 Not Found</h1></body></html>\n";
	}

	// --- HTTP/1.0 header (always close)
	char header[256];
	int header_len = std::snprintf(header, sizeof(header),
		"HTTP/1.0 %s\r\n"
		"Server: Webserv\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n"
		"Content-Length: %lu\r\n"
		"Connection: close\r\n"
		"\r\n",
		status, (unsigned long)body.size());
	if (header_len < 0) header_len = 0;
	if (header_len > (int)sizeof(header)) header_len = (int)sizeof(header);

	state = WRITING;

	// --- Send header (handle partial writes)
	size_t off = 0;
	while (off < (size_t)header_len) {
		ssize_t n = ::send(soc, header + off, (size_t)header_len - off, MSG_NOSIGNAL);
		if (n <= 0) break;
		off += (size_t)n;
	}

	// --- Send body (handle partial writes)
	off = 0;
	const char* p = body.empty() ? "" : body.c_str();
	size_t blen = body.size();
	while (off < blen) {
		ssize_t n = ::send(soc, p + off, blen - off, MSG_NOSIGNAL);
		if (n <= 0) break;
		off += (size_t)n;
	}
}
