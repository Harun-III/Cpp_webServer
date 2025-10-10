#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include "Core.hpp"

class Request;

class RequestParser
{
	public:
		RequestParser( void );
		~RequestParser( void );

		state_e		requestLineParser( Request & );
		state_e		headersParser( Request & );
		state_e		bodyParser( Request & );

	private:
		// void		fun(void);
};

#endif
