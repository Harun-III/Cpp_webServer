#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Core.hpp"
# include "Response.hpp"
# include "RequestParser.hpp"
# include "ResponseBuilder.hpp"

class Location;

class Connection
{
	public:
		Connection( void );
		~Connection( void );

		Connection( int , ServerConfig & );

		int				getSoc( void );
		int				getCode( void );
		state_e			getState( void );

		void			setCode( int );
		void			setState( state_e );

		void			requestProssessing( void );
		void			reponseProssessing( void );

	private:
		int				soc;
		State			status;

		Request			request;
		Response		response;
};

#endif
