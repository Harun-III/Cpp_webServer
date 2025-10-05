// Response.cpp

# include "core.hpp"

void Server::response(int live_sock) {
	std::cout << "Client Socket: " << live_sock << std::endl;

	// Body
	const char body[] =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"<title>Welcome to ngimax!</title>\n"
		"<style>\n"
		"html { color-scheme: light dark; }\n"
		"body { width: 35em; margin: 0 auto;\n"
		"font-family: Tahoma, Verdana, Arial, sans-serif; }\n"
		"</style>\n"
		"</head>\n"
		"<body>\n"
		"<h1>Welcome to ngimax!</h1>\n"
		"<p>If you see this page, the ngimax web server is successfully installed and\n"
		"working. Further configuration is required.</p>\n"
		"\n"
		"<p>For online documentation and support please refer to\n"
		"<a href=\"https://github.com/Harun-III/Cpp_webServer#-http-c-web-server\">\
		ngimax.com</a>.<br/>\n"
		"\n"
		"<p><em>Thank you for using ngimax.</em></p>\n"
		"</body>\n"
		"</html>\n";

	// Header (compute exact Content-Length)
	const size_t body_len = std::strlen(body);
	char header[256];
	int header_len = std::snprintf(header, sizeof(header),
		"HTTP/1.1 200 OK\r\n"
		"Server: Webserv\r\n"
		"Content-Type: text/html; charset=UTF-8\r\n"
		"Content-Length: %zu\r\n"
		"Connection: close\r\n"
		"\r\n",
		body_len);
	if (header_len < 0) header_len = 0;
	if (header_len > (int)sizeof(header)) header_len = (int)sizeof(header);

	// Send header (handle partial writes)
	size_t off = 0;
	while (off < (size_t)header_len) {
		ssize_t n = send(live_sock, header + off, (size_t)header_len - off, MSG_NOSIGNAL);
		if (n <= 0) break;
		off += (size_t)n;
	}

	// Send body (handle partial writes)
	off = 0;
	while (off < body_len) {
		ssize_t n = send(live_sock, body + off, body_len - off, MSG_NOSIGNAL);
		if (n <= 0) break;
		off += (size_t)n;
	}

	epoll_ctl(epollfd, EPOLL_CTL_DEL, live_sock, NULL);
	close(live_sock);
}
