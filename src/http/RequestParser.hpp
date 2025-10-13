#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include <sstream>
# include <iostream>
# include <algorithm>
# include "Request.hpp"

class Request;

enum state_e { REQUEST_LINE, READING_HEADERS, READING_BODY,
				READY_TO_WRITE, WRITING, CLOSING, BAD };

class State {
	public:
		State( short co , state_e st )
				{ code = co; state = st; }

		short			code;
		state_e			state;
};

class RequestParser
{
	public:
		RequestParser( void );
		~RequestParser( void );

		State		requestLineParser( Request & );
		State		headersParser( Request & );
		State		bodyParser( Request & );

	// private:
	// 	void		fun(void);
};

#endif
