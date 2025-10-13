# include "RequestParser.hpp"

RequestParser::RequestParser( void ) { }

RequestParser::~RequestParser( void ) { }

State	RequestParser::requestLineParser( Request &request ) {
	std::stringstream	streamLine;
	char				space, _space;
	size_t				pos;
	std::string			crlf("\r\n");

	if ((pos = request.recv.find(crlf)) == std::string::npos)
		return State(0, REQUEST_LINE);

	streamLine >> std::noskipws;
	streamLine << request.recv.substr(0, pos);
	request.recv.erase(0, pos);

	if (!(streamLine >> request.method >> space >> request.target >> _space >> request.version))
		return State(0, CLOSING);

	if ((request.method != "GET" && request.method != "DELETE" && request.method != "POST"))
		return State(501, BAD);

	if ((request.version != "HTTP/1.1" && request.version != "HTTP/1.0") || space != ' '
			|| _space != ' ' || request.target[0] != '/')
		return State(400, BAD);

	request.full_path = "." + request.server.getLocations().find("/")->second.getRoot()
			+ request.target; // Full Path Handler

	std::cout << "[ " << request.method <<  " ] " 
			  << "[ " << request.target <<  " ] " 
			  << "[ " << request.version <<  " ] " << std::endl;

	return State(0, READING_HEADERS);
}

State	RequestParser::headersParser( Request &request ) {
	std::string			crlf("\r\n");
	size_t				pos;

	if ((pos = request.recv.find(crlf + crlf)) == std::string::npos)
		return State(0, READING_HEADERS);

	std::stringstream	headersBlock;

	headersBlock << request.recv.substr(crlf.size(), pos);
	request.recv.erase(0, pos + (crlf + crlf).size());

	std::string			line;
	while (std::getline(headersBlock, line)) {
		if (line.empty()) continue ;

		std::string::size_type	colum = line.find(':');

		if (line[0] == ' ' || line[0] == '\t' || colum == std::string::npos)
			return State(400, BAD);

		std::string				name = line.substr(0, colum);
		std::string				value = line.substr(colum + 1);

		std::transform(name.begin(), name.end(), name.begin(),
			static_cast<int(*)(int)>(std::tolower));

		for (size_t index = 0; index < value.size() && (value[index] == ' '
				|| value[index] == '\t'); ++index)
			value.erase(0, index);

		for (size_t index = value.size(); index > 0 && (value[index - 1] == ' '
				|| value[index - 1] == '\r'); --index)
			value.erase(index - 1);

		if (name == "transfer-encoding")
			return State(400, BAD);

		std::cout << "[ " + name + " ]" << "[ " + value + " ]" << std::endl;

		request.headers[name] = value;
	}

	if (request.method == "POST")
		return State(0, READING_BODY);
	return State(0, READY_TO_WRITE);
}

State	RequestParser::bodyParser( Request &request ) {
	std::cout << request.recv << std::endl;
	return State(0, READY_TO_WRITE);
}
