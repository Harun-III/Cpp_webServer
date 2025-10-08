#ifndef LISTENER_HPP
# define LISTENER_HPP

# include "Core.hpp"

typedef struct addrinfo addrinfo_t;

class Listener
{
	public:
		Listener( const std::string & , const std::string & );
		~Listener( void );

		int						get( void );
		void					init( void );
		void					open( void );
		void					release( void );

	private:
		std::string				ip;
		int						soc;
		std::string				port;

		addrinfo_t				*res;
		addrinfo_t				hints;
};

#endif
