# include "RequestParser.hpp"

RequestParser::RequestParser( Request &request ) {
	std::string		crlf = "\r\n";

	if (request.getRecv().find(crlf) != std::string::npos)
		requestLineParser(request);
}

RequestParser::~RequestParser( void ) { }
