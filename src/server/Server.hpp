#ifndef SERVER_HPP
# define SERVER_HPP

# include "core.hpp"
# include "Listener.hpp"

typedef struct epoll_event event_t;
typedef std::vector<std::pair<std::string, std::string> > vector_pairs;

class ServerConfig;

class Server
{
	public:
		Server( void );
		~Server( void );
		
		void				run( void );
		void				create( std::vector<ServerConfig> & );

	private:
		void				create_epoll( void );
		void				socket_control( int , int , int );

		void				response( int );		// for test response

		int					epollfd;				// Epoll file descriptor
		std::vector<int>	listeners;
};

#endif
