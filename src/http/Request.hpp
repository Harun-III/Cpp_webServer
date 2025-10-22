#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Core.hpp"
# include "Location.hpp"
# include "ServerConfig.hpp"
# include "StaticFileHandler.hpp"

typedef std::map<std::string, Location>	map_location;

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
		post_e					post_dest;

		void					streamBodies( void );
		void					isValidHeaders( void );
		bool					isMethodAllowed( void );
		void					startProssessing( void );
		std::string				longestPrefixMatch( void );
};

#endif
