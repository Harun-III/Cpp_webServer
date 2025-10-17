# include "Request.hpp"

Request::Request( void ) { }

Request::Request( ServerConfig &serv ) : server(serv),
		has_content_length(false), content_length(0) { }

Request::~Request( void ) { }

std::string	joinPath ( const std::string &root, const std::string &target ) {
	if (root.empty()) return target;
	if (target.empty()) return root;

	if (root[root.size() - 1] == '/' && target[0] == '/')
		return root + target.substr(1);

	if (root[root.size() - 1] != '/' && target[0] != '/')
		return root + "/" + target;

	return root + target;
}

std::string	Request::longestPrefixMatch( void ) {
	std::string			longest = "/";
	const map_location	&locations = server.getLocations();

	for (map_location::const_iterator curr = locations.begin();
			curr != locations.end(); curr++) {
		const std::string		&prefix = curr->first;

		if (prefix.size() <= longest.size()) continue ;
		if (target.size() < prefix.size()) continue ;
		if (target.rfind(prefix, 0)) continue ;

		if (target.size() != prefix.size() && target[prefix.size()] != '/'
				&& prefix[prefix.size() - 1] != '/') continue ;

		longest = prefix;
	}

	return longest;
}

void	Request::setLocationPath( void ) {
	const map_location	&locations = server.getLocations();

	std::string		longestM = longestPrefixMatch();

	map_location::const_iterator	hit = locations.find(longestM);
	
	if (hit != locations.end()) location = hit->second;
	else location = locations.begin()->second;
	
	std::string		alias = target.substr(longestM.size());
	
	if (!alias.empty() && alias[0] != '/') alias.insert(0, "/"); // More Checks
	path = joinPath(location.getRoot(), alias);

	std::cout << path << std::endl;
}
