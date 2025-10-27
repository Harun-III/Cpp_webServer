# include "Connection.hpp"

Connection::Connection( void ) : soc(-1), status(0, REQUEST_LINE),
		last_active(Core::nowTime()) { }

Connection::Connection( int conn_sock, ServerConfig &server ) : soc(conn_sock),
		status(0, REQUEST_LINE), request(server), last_active(Core::nowTime()) { }

Connection::~Connection( void ) { }

int	Connection::getSoc( void ) { return soc; }

int	Connection::getCode( void ) { return status.code; }

state_e	Connection::getState( void ) { return status.state; }

void	Connection::setCode( int code ) { status.code = code; }

void	Connection::setState( state_e state ) { status.state = state; }

void	Connection::touch( void) { last_active = Core::nowTime(); }

time_t	Connection::getLastActive( void ) { return last_active; }

void	Connection::requestProssessing( void ) {
	char			buffer[BUF_SIZE];
	ssize_t			len;

	if ((len = recv(soc, buffer, BUF_SIZE, false)) == ERROR) return ;
	request.recv.append(buffer, len); touch();

	try {
		if (getState() == REQUEST_LINE) {
			RequestParser::requestLineParser(request);
			status = State(0, READING_HEADERS);
			touch();
		}

		if (getState() == READING_HEADERS) {
			RequestParser::headersParser(request);

			request.isValidHeaders();
			request.startProssessing();

			status = State(0, READING_BODY);
			touch();
		}

		if (getState() == READING_BODY) {
			request.streamBodies();
			touch();
		}
	}
	catch( const State &state ) { status = state; touch(); }
}

void	Connection::reponseProssessing( void ) {
	try {
		if (getState() == READY_TO_WRITE) {
			ResponseBuilder		builder(request.server);

			builder.buildResponse(request, response);
			setState(WRITING); touch();
		}
	}
	catch( const State &state ) { status = state; touch(); }

	if (getState() == WRITING) {
		// Api That Cal Function That Will Read The Resonse Body
		touch();
	}

	else if (getState() == BAD) {
		response.generateErrorPage(request.server, getCode());
		setState(CLOSING); touch();
	}

	const char				*buffer = response.generated.c_str();
	size_t					length = response.generated.length();

	send(soc, buffer, length, MSG_NOSIGNAL); touch();
}
