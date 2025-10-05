# include "Listener.hpp"

Listener::Listener( const std::string& ip, const std::string& port )
		: ip(ip), soc(ERROR), port(port), res(NULL) { }

Listener::~Listener( void ) {
	if (res != NULL) freeaddrinfo(res);
	if (soc != ERROR) close(soc);
}

int	Listener::get( void ) { return soc; }

void	Listener::release( void ) {if (soc != ERROR) soc = -1; }

void	Listener::init( void ) {
	int		er;

	bzero(&hints, sizeof(addrinfo_t));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((er = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res)) != 0)
		throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(er));
}

void	Listener::open( void ) {
	init();

	int	on  = 1;
	soc = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (soc == ERROR)
		throw std::runtime_error("<socket> " + std::string(strerror(errno)));

	if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == ERROR)
		throw std::runtime_error("<setsockopt> " + std::string(strerror(errno)));

	if(fcntl(soc, F_SETFL, O_NONBLOCK) == ERROR)
		throw std::runtime_error("<fcntl> " + std::string(strerror(errno)));

	if (bind(soc, res->ai_addr, res->ai_addrlen) == ERROR)
		throw std::runtime_error("<bind> " + std::string(strerror(errno)));

	if (listen(soc, SOMAXCONN) == ERROR)
		throw std::runtime_error("<Listen> " + std::string(strerror(errno)));
}
