#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Core.hpp"
# include "Request.hpp"

class Connection
{
	public:
		Connection( void );
		~Connection( void );

		Connection( int );

		int				getSoc( void );
		int				getCode( void );
		state_e			getState( void );

		void			setCode( int );
		void			setState( state_e );

		void			requestProssessing( void );
		// void			onSending( void );					// for response

		void			buildResponseMinimal( void );		// for test response

	private:
		int				soc;
		int				code;
		state_e			state;

		// std::string		send;							// for response

		Request			request;
};

#endif
