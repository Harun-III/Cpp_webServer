# include "RequestParser.hpp"

RequestParser::RequestParser( void ) { }

RequestParser::~RequestParser( void ) { }

state_e	RequestParser::requestLineParser( Request &request ) {
	std::stringstream	streamLine;
	char				crlf[] = "\r\n";
	size_t				pos;

	if ((pos = request.recv.find(crlf)) == std::string::npos)
		return REQUEST_LINE;

	streamLine << request.recv.substr(0, pos);
	request.recv.erase(0, pos);

	if (!(streamLine >> request.method >>  request.target >> request.version))
		return BAD;

	if (request.method != "GET" || request.method != "DELETE"
			|| request.method != "POST" || request.version != "HTTP/1.0")
		return BAD;

	request.full_path = "." + request.target;

	std::cout << "[ " << request.method <<  " ] " 
			  << "[ " << request.target <<  " ] " 
			  << "[ " << request.version <<  " ] " << std::endl;

	return READING_HEADERS;
}

state_e	RequestParser::headersParser( Request &request ) {
	std::cout << request.recv << std::endl;
	return READING_BODY;
}

state_e	RequestParser::bodyParser( Request &request ) {
	(void) request;
	return READY_TO_WRITE;
}
