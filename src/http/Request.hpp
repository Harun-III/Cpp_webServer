#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <vector>
# include <iostream>
# include "ConfigParser.hpp"
# include "Location.hpp"

typedef std::map<std::string, std::string> map_t;
typedef std::map<std::string, Location> map_location;

class Request
{
	public:
		Request( void );
		~Request( void );

		Request( ServerConfig &server );
		
		std::string				longestPrefixMatch( void );
		void					setLocationPath( void);

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

		void					start( void );
};

#endif
