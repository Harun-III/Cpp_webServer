#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <vector>
# include <iostream>
# include "ConfigParser.hpp"
# include "Location.hpp"

typedef std::map<std::string, std::string> map_t;
typedef std::map<std::string, Location> map_location;

enum state_e { REQUEST_LINE, READING_HEADERS, READING_BODY,
				READY_TO_WRITE, WRITING, CLOSING, BAD };

class State {
	public:
		State( short co , state_e st )
				{ code = co; state = st; }

		short			code;
		state_e			state;
};

class Request
{
	public:
		Request( void );
		~Request( void );

		Request( ServerConfig &server );

		std::string				recv;
		std::string				path;
		std::string				query;

		std::string				method;
		std::string				target;
		std::string				version;

		map_t					headers;
		Location				location;
		ServerConfig			server;

		bool					has_content_length;
		size_t					content_length;

		std::string				longestPrefixMatch( void );
		State					startProssessing( void );
		bool					isMethodAllowed( void );
};

#endif
