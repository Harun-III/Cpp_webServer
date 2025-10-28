# include "Request.hpp"

Request::Request( void ) : content_length(0), detectRoute(NONE) { }

Request::Request( ServerConfig &serv ) : server(serv), content_length(0),
		detectRoute(NONE) { }

Request::~Request( void ) { }

bool	Request::isMethodAllowed( void ) {
	const std::vector<std::string>	&allowed = location.getMethods();

	if (allowed.empty()) return (method == "GET");

	for (size_t index = 0; index < allowed.size(); ++index)
		if (allowed[index] == method) return true;

	return false;
}

std::string	Request::joinPath ( const std::string &root, const std::string &target ) const {
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

std::string	Request::generateUniqueName( void ) {
	std::time_t			wallTime = std::time(NULL);
	std::clock_t		cpuTicks = std::clock();

	std::ostringstream	orand;
	orand << "upload_" << wallTime << "_" << cpuTicks
		<< extMime.getExtension(content_type);

	return orand.str();
}

void	Request::isValidHeaders( void ) {
	if (headers.find("transfer-encoding") != headers.end())
		throw State(400, BAD);

	if (method != "POST") return ;

	map_s::iterator		headerIter = headers.find("content-length");
	if (headerIter == headers.end()) throw State(411, BAD);

	std::string			&value = headerIter->second;
	if (value.empty()) throw State(400, BAD);

	if (value.find_first_not_of("0123456789") != std::string::npos)
		throw State(400, BAD);

	std::stringstream		convert(value);

	convert >> content_length;
	if (convert.fail()) throw State(400, BAD);

	size_t	max_size = server.getMaxClientBodySize();
	if (content_length > max_size) throw State(413, BAD);

	headerIter = headers.find("content-type");
	if (headerIter == headers.end()) throw State(400, BAD);

	content_type = headerIter->second;
}

bool	Request::isCgiRequest( void ) const {
	size_t							dot_pos;
	std::string						extension;

	if ((dot_pos = target.find_last_of(".")) == std::string::npos)
		return false;

	extension = target.substr(dot_pos);
	return location.getCgi().find(extension) != location.getCgi().end();
}

void	Request::routePost( const std::string &longestM ) {
	detectRoute = UPLOAD;
	if (detectRoute == CGI)
		cgiPath = joinPath(location.getUploadLocation(), generateUniqueName());
	else if (location.getUpload() == true) {
		detectRoute = UPLOAD;
		path = target.substr(longestM.size());
		path = joinPath(location.getUploadLocation(), path);

		if (fileHandler.isDirectory(path))
			path = joinPath(path, generateUniqueName());

		if (fileHandler.fileExists(path)) fileHandler.deleteFile(path);
	}
	else throw State(403, BAD);
}

void	Request::startProssessing( void ) {
	const map_location				&locations = server.getLocations();
	std::string						longestM = longestPrefixMatch();
	map_location::const_iterator	hit = locations.find(longestM);

	if (hit != locations.end()) location = hit->second;
	else throw State(404, BAD);

	if (hit->second.getReturn().first) {
		detectRoute = REDIR; throw State(0, READY_TO_WRITE);
	}

	if (!isMethodAllowed()) throw State(405, BAD);

	if (isCgiRequest()) detectRoute = CGI;

	if (method != "POST") {
		path = target.substr(longestM.size());
		path = joinPath(location.getRoot(), path);

		std::cout << "[ " << longestM << " ]"
				<< "[ " << content_length << " ]"
					"[ " << path << " ]" << std::endl;

		throw State(0, READY_TO_WRITE);
	}

	if (method == "POST") routePost(longestM);

	std::cout << "[ " << longestM << " ]"
			  << "[ " << content_length << " ]"
				 "[ " << path << " ]" << std::endl;
}

void	Request::streamBodies( void ) {
	if (content_length > 0 && recv.empty()) throw State(0, READING_BODY);
	std::cout << "Streaming ..." << std::endl;

	std::string		filePath;
	size_t			to_write;

	if (detectRoute == CGI) filePath = cgiPath;
	else if (detectRoute == UPLOAD) filePath = path;
	else throw State(500, BAD); 

	if ((to_write = std::min(recv.size(), content_length)) > 0) {
		std::ofstream	outfile(filePath.c_str(), std::ios::binary | std::ios::app);
		if (!outfile.is_open()) throw State(500, BAD);

		outfile.write(recv.data(), to_write);
		if (!outfile.good()) { outfile.close(); throw State(500, BAD); }

		recv.erase(0, to_write); content_length -= to_write;
		outfile.close();
	}

	if (content_length == 0) {
		if (detectRoute == CGI && (cgiFd = open(filePath.c_str(), O_RDONLY)) == -1)
			throw State(500, BAD);

		if (detectRoute == CGI) std::remove(filePath.c_str());
		throw State(0, READY_TO_WRITE);
	}
}
