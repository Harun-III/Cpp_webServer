#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include "Core.hpp"

class Request;

class RequestParser
{
	public:
		RequestParser( void );
		~RequestParser( void );
		
		RequestParser( Request & );

		bool		requestLineParser( Request & );
};

#endif
