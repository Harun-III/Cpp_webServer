#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Core.hpp"
# include "Request.hpp"
# include "HttpResponse.hpp"
# include "HttpResponseBuilder.hpp"

enum state_e { REQUEST_LINE, READING_HEADERS, READING_BODY,
				READY_TO_WRITE, WRITING, CLOSING, BAD };

class Location;

class Connection
{
	public:
		Connection( void );
		~Connection( void );

		Connection( int , ServerConfig );

		int				getSoc( void );
		int				getCode( void );
		state_e			getState( void );

		void			setCode( int );
		void			setState( state_e );

		void			requestProssessing( void );
		void			reponseProssessing( void );

	private:
		int				soc;
		int				code;
		state_e			state;

		Request			request;
		ServerConfig	server;
		HttpResponse	response;
};

#endif
