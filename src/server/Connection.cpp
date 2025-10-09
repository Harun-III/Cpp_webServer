# include "Connection.hpp"
# include "RequestParser.hpp"

Connection::Connection( void ) : soc(-1), code(0), state(REQUEST_LINE) { }

Connection::Connection( int conn_sock, ServerConfig server ) : soc(conn_sock),
		code(0), state(REQUEST_LINE), server(server) { }

Connection::~Connection( void ) { }

int	Connection::getSoc( void ) { return soc; }

int	Connection::getCode( void ) { return code; }

state_e	Connection::getState( void ) { return state; }

void	Connection::setCode( int code ) { this->code = code; }

void	Connection::setState( state_e state ) { this->state = state; }

void	Connection::requestProssessing( void ) {
	RequestParser	parser;
	char			buffer[BUF_SIZE];
	ssize_t			len;
	
	if ((len = recv(soc, buffer, BUF_SIZE, false)) == ERROR) return ;

	request.recv.append(buffer, len);

	if (state == REQUEST_LINE) state = parser.requestLineParser(request);
	if (state == READING_HEADERS) state = parser.headersParser(request);
	if (state == READING_BODY) state = parser.bodyParser(request);
}

void	Connection::reponseProssessing( void ) {
	HttpResponseBuilder		builder(server);

	response = builder.buildResponse(request);
	send(soc, response.toString().c_str(), response.toString().length(), MSG_NOSIGNAL);
}
