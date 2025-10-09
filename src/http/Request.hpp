#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <vector>
# include "Location.hpp"

typedef std::vector< std::pair<std::string, std::string> > vector_pairs;

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
		std::string				body;

		vector_pairs			headers;

		bool					has_content_length;
		size_t					content_length;
};

#endif
