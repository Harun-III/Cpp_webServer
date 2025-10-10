#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <vector>
# include "Location.hpp"

typedef std::map<std::string, std::string> map_t;

class Request
{
	public:
		Request( void );
		~Request( void );

		std::string				recv;

		std::string				method;
		Location				location;
		std::string				full_path;

		std::string				target;
		std::string				version;
		std::string				query;

		map_t					headers;

		bool					has_content_length;
		size_t					content_length;
};

#endif
