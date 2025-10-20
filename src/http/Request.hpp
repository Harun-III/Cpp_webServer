#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <vector>
# include <fstream>
# include <iostream>
# include "ConfigParser.hpp"
# include "Location.hpp"
# include "StaticFileHandler.hpp"

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

		ServerConfig			server;
		map_t					headers;
		Location				location;

		bool					has_conlen;
		size_t					content_length;

		State					streamBodies( void );
		// bool					isValidHeaders( void );
		bool					isMethodAllowed( void );
		State					startProssessing( void );
		std::string				longestPrefixMatch( void );
};

#endif
