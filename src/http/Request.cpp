# include "Request.hpp"

Request::Request( void ) { }

Request::Request( ServerConfig &serv ) : server(serv),
		has_conlen(false), content_length(0) { }

Request::~Request( void ) { }

bool	Request::isMethodAllowed( void ) {
	const std::vector<std::string>	&allowed = location.getMethods();

	if (allowed.empty()) return (method == "GET");

	for (size_t index = 0; index < allowed.size(); ++index)
		if (allowed[index] == method) return true;

	return false;
}

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
	std::string						longest = "/";
	const map_location				&locations = server.getLocations();

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

// State	Request::isValidHeaders( void ) {
// 	if (headers.find("transfer-encoding") != headers.end())
// 		return State(400, BAD);

// 	if (method != "POST") return State(0, READY_TO_WRITE);

// 	map_t::iterator		headerIter = headers.find("content-length");

// 	if (headerIter == headers.end()) return State(400, BAD);

// 	std::string		value = headerIter->second;

// 	if (value.empty()) return State(400, BAD);

// 	if (value.find_first_not_of("0123456789") != std::string::npos)
// 		return State(400, BAD);

// 	std::stringstream		convert(value);

// 	has_conlen = true;
// 	convert >> content_length;
// 	if (convert.fail()) return State(400, BAD);

// 	size_t	max_size = server.getMaxClientBodySize();
// 	if (content_length > max_size) return State(413, BAD);

// 	// headerIter = headers.find("content-type");
// 	// if (headerIter != headers.end()) {
// 	// 	;
// 	// }

// 	return State(0, READING_BODY);
// }

State	Request::startProssessing( void ) {
	const map_location				&locations = server.getLocations();
	std::string						longestM = longestPrefixMatch();
	map_location::const_iterator	hit = locations.find(longestM);

	if (hit != locations.end()) location = hit->second;
	else return State(404, BAD);

	if (!isMethodAllowed()) return State(405, BAD);

	path = target.substr(longestM.size());
	path = joinPath(location.getRoot(), path);

	std::cout << "[ " << longestM << " ]"
				 "[ " << path << " ]" << std::endl;

	return State(0, READING_HEADERS);
}

State	Request::streamBodies( void ) {
	std::ofstream		outfile(path.c_str());

	if (!outfile.is_open()) return State(500, BAD);

	if (recv.length() >= content_length) {
		outfile << recv;
		content_length -= recv.length();
	}
	else if (!content_length) {
		outfile << recv.substr(0, content_length);
		return (outfile.close(), State(0, READY_TO_WRITE));
	}

	return State(0, READING_BODY);
}
