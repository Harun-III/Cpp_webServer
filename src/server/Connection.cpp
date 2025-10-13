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

	RequestParser	parser;
	request.recv.append(buffer, len);

	if (getState() == REQUEST_LINE)
		status = parser.requestLineParser(request);
	if (getState() == READING_HEADERS)
		status = parser.headersParser(request);
	if (getState() == READING_BODY)
		status = parser.bodyParser(request);
}

void	Connection::reponseProssessing( void ) {
	ResponseBuilder		builder(request.server);

	if (getState() != BAD) 
		response = builder.buildResponse(request);
	
	if (getState() == BAD)
		response.generateErrorPage(request.server, status.code);

	send(soc, response.toString().c_str(), response.toString().length(), MSG_NOSIGNAL);
	setState(CLOSING);
}
