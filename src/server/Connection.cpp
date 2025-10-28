# include "Connection.hpp"

Connection::Connection( void ) : soc(-1), status(0, REQUEST_LINE),
		last_active(std::time(NULL)) { }

Connection::Connection( int conn_sock, ServerConfig &server ) : soc(conn_sock),
		status(0, REQUEST_LINE), request(server), last_active(std::time(NULL)) { }

Connection::~Connection( void ) { }

int	Connection::getSoc( void ) { return soc; }

int	Connection::getCode( void ) { return status.code; }

state_e	Connection::getState( void ) { return status.state; }

void	Connection::setCode( int code ) { status.code = code; }

void	Connection::setState( state_e state ) { status.state = state; }

void	Connection::touch( void) { last_active = std::time(NULL); }

time_t	Connection::getLastActive( void ) { return last_active; }

void	Connection::sending( void ) {
	if (response.generated.empty()) return;

	const char		*buffer = response.generated.data();
	size_t			to_send = std::min(response.generated.size(), (size_t)BUF_SIZE);

	ssize_t			readed = send(soc, buffer, to_send, MSG_NOSIGNAL); touch();
	if (readed > 0) { response.generated.erase(0, to_send); return ; }

	setCode(500); setState(BAD); touch();
}

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
	if (getState() == READY_TO_WRITE) {
		try {
			ResponseBuilder		builder(request.server);

			builder.buildResponse(request, response);
			setState(WRITING); touch();
		}
		catch( const State &state ) { status = state; touch(); }
	}

	if (getState() == WRITING) {
		if (response.continueStreaming() && response.generated.empty()) {
			setState(CLOSING); touch(); return ;
		} touch();
	}

	if (getState() == BAD) {
		if (response.bodyStream.is_open()) response.bodyStream.close();

		response.generated.clear();
		response.generateErrorPage(request.server, getCode());
		setState(WRITING); touch();
	}

	if (getState() == WRITING) sending();
}
