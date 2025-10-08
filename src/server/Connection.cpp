# include "Connection.hpp"
# include "RequestParser.hpp"

Connection::Connection( void ) : soc(-1), code(0),
		state(REQUEST_LINE) { }

Connection::Connection( int conn_sock ) : soc(conn_sock), code(0),
		state(REQUEST_LINE) { }

Connection::~Connection( void ) { }

int	Connection::getSoc( void ) { return soc; }

int	Connection::getCode( void ) { return code; }

state_e	Connection::getState( void ) { return state; }

void	Connection::setCode( int code ) { this->code = code; }

void	Connection::setState( state_e state ) { this->state = state; }

void			Connection::requestProssessing( void ) {
	char			buffer[BUF_SIZE];

	size_t		len = recv(soc, buffer, BUF_SIZE, MSG_NOSIGNAL);

	request.getRecv().append(buffer, len);

	RequestParser(request);

	if (state == READY_TO_WRITE)
		socket_control(getSoc(), EPOLLOUT, EPOLL_CTL_MOD);
}
