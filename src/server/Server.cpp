# include "Server.hpp"

Server::Server( void ) : epfd(ERROR) { }

Server::~Server( void ) {
	for (std::map<int, ServerConfig>::iterator curr = listeners.begin();
			curr != listeners.end(); curr++)
		close(curr->first);
	if (epfd != ERROR) close(epfd);
}

void	Server::create_epoll( void ) {
	if ((epfd = epoll_create(MAX_EVENTS)) == ERROR)
		throw std::runtime_error("<epoll_create> " + std::string(strerror(errno)));
}

bool	Server::onReading( int sock ) {
	state_e		state = connections.find(sock)->second->getState();

	return state == REQUEST_LINE || state == READING_HEADERS
				|| state == READING_BODY;
}

bool	Server::onWriting( int sock ) {
	state_e		state = connections.find(sock)->second->getState();

	return state == READY_TO_WRITE || state == WRITING
				|| state == CLOSING || state == BAD;
}

void	Server::close_connection( int sock ) {
	epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
	delete connections.find(sock)->second;
	connections.erase(sock);
	close(sock);
}

void	Server::socket_control( int fd, int mode, int op ) {
	event_t		ev;

	bzero(&ev, sizeof(event_t));
	ev.events = mode, ev.data.fd = fd;

	if (epoll_ctl(epfd, op, ev.data.fd, &ev) == ERROR)
		throw std::runtime_error("<epoll_ctl> " + std::string(strerror(errno)));
}		

void	Server::accept_connection( int sock ) {
	int		conn_sock = accept(sock, NULL, NULL);

	if (conn_sock == ERROR) return ;

	fcntl(conn_sock, F_SETFL, O_NONBLOCK);
	connections[conn_sock] = new Connection(conn_sock, listeners.find(sock)->second);
	socket_control(conn_sock, EPOLLIN | EPOLLRDHUP, EPOLL_CTL_ADD);
}

void	Server::check_timeouts( void ) {
	time_t			now = std::time(NULL);

	for (std::map<int, Connection *>::iterator loop = connections.begin();
		loop != connections.end(); ++loop) {

		Connection	*conn = loop->second;
		if (conn->getState() == BAD || conn->getState() == CLOSING ) continue;
		if (now - conn->getLastActive() >= TIMEOUT) {
			conn->setCode(408); conn->setState(BAD);
			socket_control(conn->getSoc(), EPOLLOUT, EPOLL_CTL_MOD);
		}
	}
}

void	Server::create( const std::vector<ServerConfig> &servers ) {
	create_epoll();

	for (std::vector<ServerConfig>::const_iterator current = servers.begin();
			current != servers.end(); current++)
	{
		const vector_pairs	&listen = current->getListen();
		for (vector_pairs::const_iterator curr_lis = listen.begin();
				curr_lis != listen.end(); curr_lis++) {

			const std::string	&ip   = curr_lis->first;
			const std::string	&port = curr_lis->second;

			Listener	listener(ip, port);

			listener.open();
			socket_control(listener.get(), EPOLLIN, EPOLL_CTL_ADD);

			std::cout << GR "[INFO] Server " << ip << " listening on port "
				<< port + RS << std::endl;

			listeners[listener.get()] = *current;
			listener.release();
		}
	}
}

void	Server::run( void )
{
	std::cout << GR "[SUCCESS] Server started successfully!" RS << std::endl;

	while ( true ) {
		if ((nfds = epoll_wait(epfd, events, MAX_EVENTS, TIMEOUT)) == ERROR)
			throw std::runtime_error("<epoll_wait> " + std::string(strerror(errno)));

		for (int curr_ev = 0; curr_ev < nfds; curr_ev++) {
			int			curr_sock = events[curr_ev].data.fd;

			if (listeners.find(curr_sock) != listeners.end()) {
				accept_connection(curr_sock);
				continue ;
			}

			Connection		*connection = connections.find(curr_sock)->second;

			if ((events[curr_ev].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
					|| connection->getState() == CLOSING) {
				close_connection(curr_sock); continue ;
			}

			if (events[curr_ev].events & EPOLLIN) {
				if (onReading(curr_sock) == true) connection->requestProssessing();
				if (onWriting(curr_sock) == true)
					socket_control(connection->getSoc(), EPOLLOUT, EPOLL_CTL_MOD);
				continue ;
			}

			if (events[curr_ev].events & EPOLLOUT)
				if (onWriting(curr_sock) == true) connection->reponseProssessing();

			if (connection->getState() == CLOSING) {
				close_connection(curr_sock); continue ;
			}
		}
		check_timeouts();
	}
}
