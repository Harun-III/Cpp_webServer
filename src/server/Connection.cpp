# include "Connection.hpp"

Connection::Connection( void ) : soc(-1), status(0, REQUEST_LINE),
		last_active(Core::now_ms()) { }

Connection::Connection( int conn_sock, ServerConfig &server ) : soc(conn_sock),
		status(0, REQUEST_LINE), request(server), last_active(Core::now_ms()) { }

Connection::~Connection( void ) { }

int	Connection::getSoc( void ) { return soc; }

int	Connection::getCode( void ) { return status.code; }

state_e	Connection::getState( void ) { return status.state; }

void	Connection::setCode( int code ) { status.code = code; }

void	Connection::setState( state_e state ) { status.state = state; }

void	Connection::touch( void) { last_active = Core::now_ms(); }

time_t	Connection::getLastActive( void ) { return last_active; }

void	Connection::requestProssessing( void ) {
	char			buffer[BUF_SIZE];
	ssize_t			len;

	if ((len = recv(soc, buffer, BUF_SIZE, false)) == ERROR) return ;
	request.recv.append(buffer, len); touch();

	try {

		if (getState() == REQUEST_LINE) {
			RequestParser::requestLineParser(request);
			status = State(0, READING_HEADERS); touch();
		}

		if (getState() == READING_HEADERS) {
			RequestParser::headersParser(request);

			request.isValidHeaders();
			request.startProssessing();

			status = State(0, READING_BODY); touch();
		}

		if (getState() == READING_BODY) {
			request.streamBodies(); touch();
		}

	}
	catch( const State &state ) { status = state; }
}

void	Connection::reponseProssessing( void ) {
	ResponseBuilder		builder(request.server);

	std::cout << std::setw(40) << std::left
		<< GR "Code: [ " << getCode() << " ]" RS << std::endl;

	if (getState() != BAD)
		response = builder.buildResponse(request);

	if (getState() == BAD)
		response.generateErrorPage(request.server, getCode());

	send(soc, response.toString().c_str(),
		response.toString().length(), MSG_NOSIGNAL);

	touch(); setState(CLOSING);
}
