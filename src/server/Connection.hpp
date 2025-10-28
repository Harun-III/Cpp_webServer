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

		void			touch( void );
		time_t			getLastActive( void );

		void			requestProssessing( void );
		void			reponseProssessing( void );
		void			sending( void );

	private:
		Connection( const Connection & );
		Connection& operator=( const Connection & );

		int				soc;
		State			status;

		Request			request;
		Response		response;
		time_t			last_active;
};

#endif
