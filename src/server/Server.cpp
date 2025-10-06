# include "Server.hpp"

Server::Server( void ) : epoll_fd(ERROR) { }

Server::~Server( void ) {
	std::cout << "Server Closed ...\n";

	for (size_t index = 0; index < listeners.size(); index++)
		close(listeners[index]);
	if (epoll_fd != ERROR) close(epoll_fd);
}

void	Server::create_epoll( void ) {
	if ((epoll_fd = epoll_create(MAX_EVENTS)) == ERROR)
		throw std::runtime_error("<epoll_create> " + std::string(strerror(errno)));
}

void	Server::socket_control( int fd, int mode, int op ) {
	event_t		ev;

	bzero(&ev, sizeof(event_t));
	ev.events = mode, ev.data.fd = fd;

	if (epoll_ctl(epoll_fd, op, ev.data.fd, &ev) == ERROR)
		throw std::runtime_error("<epoll_ctl> " + std::string(strerror(errno)));
}

void	Server::close_connection( int sock ) {
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock, NULL);
	connections.erase(sock);
	close(sock);
}

void	Server::accept_connection( int sock )
{
	int		conn_sock = accept(sock, NULL, NULL);

	if (conn_sock == ERROR)
		throw std::runtime_error("<accept> " + std::string(strerror(errno)));

	fcntl(conn_sock, F_SETFL, O_NONBLOCK);
	connections[conn_sock] = Connection(conn_sock);
	socket_control(conn_sock, EPOLLIN | EPOLLRDHUP, EPOLL_CTL_ADD);
}

void	Server::create( std::vector<ServerConfig> &servers ) {
	create_epoll();

	for (std::vector<ServerConfig>::iterator current = servers.begin();
			current != servers.end(); current++)
	{
		const vector_pairs	&listen = current->getListen();
		for (vector_pairs::const_iterator curr_lis = listen.begin();
				curr_lis != listen.end(); curr_lis++) {

			const std::string	&ip   = curr_lis->first;
			const std::string	&port = curr_lis->second;

			std::cout << "Server Running " << ip << " - port:" << port << " ...\n";

			Listener	listener(ip, port);

			listener.open();
			socket_control(listener.get(), EPOLLIN, EPOLL_CTL_ADD);
			listeners.push_back(listener.get());
			listener.release();
		}
	}
}

void	Server::run( void )
{
	int				nfds;
	event_t			events[MAX_EVENTS];

	while ( true ) {
		if ((nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == ERROR)
			throw std::runtime_error("<epoll_wait> " + std::string(strerror(errno)));

		for (int curr_ev = 0; curr_ev < nfds; curr_ev++) {
			int			curr_sock = events[curr_ev].data.fd;

			if (find(listeners.begin(), listeners.end(), curr_sock) != listeners.end()) {
				accept_connection(curr_sock);
				continue ;
			}

			if (events[curr_ev].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
				close_connection(curr_sock);
				continue ;
			}

			if (events[curr_ev].events & EPOLLIN) {
				if (connections[curr_sock].getState() == READING_HEADERS)
					connections[curr_sock].requestProssessing();
				continue ;
			}

			if (events[curr_ev].events & EPOLLOUT) {
				if (connections[curr_sock].getState() == READY_TO_WRITE)
					connections[curr_sock].buildResponseMinimal();
				if (connections[curr_sock].getState() == CLOSING)
					close_connection(curr_sock);
			}
		}
	}
}
