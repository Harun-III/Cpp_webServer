# include "Connection.hpp"

Connection::Connection( void ) : soc(-1), status(0, REQUEST_LINE) { }

Connection::Connection( int conn_sock, ServerConfig &server ) : soc(conn_sock),
		status(0, REQUEST_LINE){

	request.server = server;
	request.location = server.getLocations().find("/")->second;
}

Connection::~Connection( void ) { }

int	Connection::getSoc( void ) { return soc; }

int	Connection::getCode( void ) { return status.code; }

state_e	Connection::getState( void ) { return status.state; }

void	Connection::setCode( int code ) { status.code = code; }

void	Connection::setState( state_e state ) { status.state = state; }

void	Connection::requestProssessing( void ) {
	char			buffer[BUF_SIZE];
	ssize_t			len;

	if ((len = recv(soc, buffer, BUF_SIZE, false)) == ERROR) return ;
	request.recv.append(buffer, len);
	
	std::cout << "\x1b[2J\x1b[H" << std::flush;

	if (getState() == REQUEST_LINE)
		status = RequestParser::requestLineParser(request);
	if (getState() == READING_HEADERS)
		status = RequestParser::headersParser(request);
	if (getState() == READING_BODY)
		status = RequestParser::bodyParser(request);
}

void	Connection::reponseProssessing( void ) {
	ResponseBuilder		builder(request.server);

	std::cout << std::setw(40) << std::left
		<< "\e[1;33mCode: [ " << getCode() << " ]\033[0m" << std::endl;

	if (getState() != BAD) 
		response = builder.buildResponse(request);
	
	if (getState() == BAD)
		response.generateErrorPage(request.server, status.code);

	send(soc, response.toString().c_str(), response.toString().length(), MSG_NOSIGNAL);
	setState(CLOSING);
}
