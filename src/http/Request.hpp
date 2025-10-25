#ifndef REQUEST_HPP
# define REQUEST_HPP

// # include "Post.hpp"
# include "Core.hpp"
# include "Location.hpp"
# include "MimeResolver.hpp"
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
		map_s					headers;
		Location				location;

		size_t					content_length;
		std::string				content_type;

		int						cgiFd;
		std::string				cgiPath;
		post_e					detectPost;

		// Post					;

		MimeResolver			extMime;
		StaticFileHandler		fileHandler;

		void					isValidHeaders( void );
		bool					isMethodAllowed( void );
		void					startProssessing( void );
		std::string				longestPrefixMatch( void );

		bool					isCgiRequest( void ) const;
		void					streamBodies( void );
		std::string				generateUniqueName( void );
};

#endif
