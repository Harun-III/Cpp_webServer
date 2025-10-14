#ifndef SERVER_HPP
# define SERVER_HPP

# include "Core.hpp"
# include "Listener.hpp"
# include "Connection.hpp"

typedef struct epoll_event event_t;

class Connection;

class ServerConfig;

class Server
{
	public:
		Server( void );
		~Server( void );
		
		void							run( void );
		void							create( std::vector<ServerConfig> & );

	private:
		void							create_epoll( void );
		void							socket_control( int , int , int );
		void							accept_connection( int );
		void							close_connection( int );

		bool							onReading( int );
		bool							onWriting( int );

		int								epoll_fd;
		std::map<int, ServerConfig>		listeners;
		std::map<int, Connection>		connections;
};

#endif
