#ifndef LISTENER_HPP
# define LISTENER_HPP

# include "Core.hpp"

class Listener
{
	public:
		Listener( const std::string & , const std::string & );
		~Listener( void );

		int						get( void );
		void					open( void );
		void					release( void );

	private:
		std::string				ip;
		int						soc;
		std::string				port;

		addrinfo_t				*res;
		addrinfo_t				hints;

		void					init( void );
};

#endif
