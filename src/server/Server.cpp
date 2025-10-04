# include "Server.hpp"

Server::Server( std::vector<ServerConfig> &servers ) {
	create_epoll();

	for (std::vector<ServerConfig>::iterator current = servers.begin();
			current != servers.end(); current++)
	{
		const std::vector<std::pair<std::string, std::string> >	&listen = current->getListen();
		for (std::vector<std::pair<std::string, std::string> >::const_iterator curr_lis = listen.begin();
				curr_lis != listen.end(); curr_lis++) {

			const std::string	ip   = curr_lis->first;
			const std::string	port = curr_lis->second;

			std::cout << "Server Running " << ip << " - port:" << port << " ...\n";

			try {
				Listener	listen_socket(ip, port);

				listen_socket.addrinfo_init();
				listen_socket.open_listener();
				socket_control(listen_socket.getFd(), EPOLLIN, EPOLL_CTL_ADD);
				listeners.push_back(listen_socket.getFd());
			}
			catch( const std::runtime_error &error ) {
				Server::~Server();
				throw error;
			}
		}
	}
}

void	Server::socket_control( int fd, int mode, int op ) {
	event_t		ev;

	bzero(&ev, sizeof(event_t));
	ev.events = mode;
	ev.data.fd = fd;

	if (epoll_ctl(epollfd, op, ev.data.fd, &ev) == -1)
		throw std::runtime_error("<epoll_ctl> " + std::string(strerror(errno)));
}

void	Server::create_epoll( void ) {
	if ((epollfd = epoll_create(MAX_EVENTS)) == -1)
		throw std::runtime_error("<epoll_create> " + std::string(strerror(errno)));;
}

Server::~Server( void ) {
	std::cout << "Server Closed ...\n";
	close(epollfd);
}

void  Server::run( void )
{
	int				nfds;
	event_t			events[MAX_EVENTS];

	while ( true ) {
		if ((nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1)) == -1)
			perror("Epoll_wait");

		for (int curr_ev = 0; curr_ev < nfds; curr_ev++) {
			event_t							&live_ev = events[curr_ev];
			int								live_sock = live_ev.data.fd;
			std::vector<int>::iterator		server_soc = find(listeners.begin(), listeners.end(), live_sock);

			if (server_soc != listeners.end()) {
				int		conn_sock = accept(*server_soc, NULL, NULL);

				fcntl(conn_sock, F_SETFL, O_NONBLOCK);
				socket_control(conn_sock, EPOLLIN, EPOLL_CTL_ADD);
				continue ;
			}

			if (live_ev.events & EPOLLIN) {
				socket_control(live_sock, EPOLLOUT, EPOLL_CTL_MOD);
				continue ;
			}

			if (live_ev.events & EPOLLOUT)
				response(live_sock);
		}
	}
}
