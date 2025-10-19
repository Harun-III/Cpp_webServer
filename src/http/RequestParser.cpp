# include "Core.hpp"
# include "RequestParser.hpp"

bool	RequestParser::isCTL( unsigned char c ) {
	return (c <= 31) || (c == 127);
}

bool	RequestParser::isValidMethod( const std::string &method ) {
	return method == "GET" || method == "DELETE" || method == "POST";
}

bool RequestParser::containsChar( char c, const std::string& set ) {
	return set.find(c) != std::string::npos;
}

bool RequestParser::containsAny( const std::string &str, const std::string &set ) {
	return str.find_first_of(set) != std::string::npos;
}

bool RequestParser::containsOnly( const std::string &str, const std::string &set ) {
	return !str.empty() && str.find_first_not_of(set) == std::string::npos;
}

std::string	RequestParser::strToUpper( std::string token ) {
	for (size_t index = 0; index < token.size(); ++index)
		token[index] = std::toupper(token[index]);
	return token;
}

char	RequestParser::hexToChar( const std::string &hex ) {
	unsigned int			value = 0;
	std::stringstream		streamHex(hex);

	streamHex >> std::hex >> value;
	return static_cast<char>(value);
}

bool	RequestParser::isTspecials( unsigned char c ) {
	switch (c) {
		case '(': case ')': case '<': case '>': case '@':
		case ',': case ';': case ':': case '\\': case '\"':
		case '/': case '[': case ']': case '?': case '=':
		case '{': case '}': case ' ': case '\t':
			return true;
		default:
			return false;
	}
}

bool	RequestParser::methodParser( std::string &method, int &code) {
	code = 400;

	if (method.empty()) return false;
	
	for (size_t index = 0; index < method.size(); ++index) {
		if (isTspecials(method[index]) || isCTL(method[index]))
			return false;
	}

	if (isValidMethod(method)) return true;
	if (isValidMethod(strToUpper(method))) return false;
	
	return (code = 501, false);
}

bool	RequestParser::decodeValidator( std::string &string, bool is_query ) {
	std::string		decoded;

	for (size_t index = 0; index < string.size(); ++index) {
		char		curr = string[index];

		if (containsChar(curr, " \t\\#")) return false;

		if (curr == '%') {
			if (index + 2 >= string.size()) return false;
			if (!std::isxdigit(string[index + 1])) return false;
			if (!std::isxdigit(string[index + 2])) return false;
	
			curr = hexToChar(string.substr(index + 1, 2));
			index += 2;
		}

		if (is_query && isCTL(curr) && curr != '\t') return false;
		if (!is_query && isCTL(curr)) return false;
		if (!is_query && containsChar(curr, " \t\\#")) return false;

		decoded += curr;
	}

	return (string = decoded, true);
}

std::string	RequestParser::removeDotSegment( std::string in ) {
	std::string			out;

	while (!in.empty()) {
		if (!in.rfind("./", 0)) in.erase(0, 2);
		else if (!in.rfind("../", 0)) in.erase(0, 3);

		else if (!in.rfind("/./", 0) || in == "/.")
			in.replace(0, (in == "/.") ? 2 : 3, "/");

		else if (!in.rfind("/../", 0) || in == "/..") {
			in.replace(0, (in == "/..") ? 3 : 4, "/");

			size_t		pos = out.find_last_of('/');
			if (pos != std::string::npos) out.erase(pos);
			else throw ERROR;
		}

		else if (in == "." || in == "..")
			in.clear();

		else {
			size_t		pos = in.find('/', 1);
			if (pos == std::string::npos) pos = in.size();

			out.append(in, 0, pos);
			in.erase(0, pos);
		}
	}

	return out;
}

bool	RequestParser::targetParser( std::string &target, std::string &query ) {
	if(target.empty() || target[0] != '/') return false;

	size_t		pos;

	if((pos = target.find('?')) != std::string::npos) {
		query = target.substr(pos + 1);
		target.erase(pos);
	}

	if (decodeValidator(target, false) == false) return false;
	if (decodeValidator(query, true) == false) return false;

	try {
		if (target.find('.') != std::string::npos)
			target = removeDotSegment(target);
		return true;
	}

	catch ( int ) { return false; }
}

bool	RequestParser::versionParser( const std::string &version, int &code ) {
	code = 400;

	const std::string		prefix = "HTTP/";
	const std::string		nums = "0123456789";

	if (version.size() < prefix.size() + 3) return false;
	if (version.compare(0, prefix.size(), prefix)) return false;

	size_t			dot = version.find('.', prefix.size());
	if (dot == std::string::npos) return false;

	std::string		major = version.substr(prefix.size(), dot - prefix.size());
	std::string		minor = version.substr(dot + 1);

	if (containsOnly(major, nums) == false || containsOnly(minor, nums) == false)
		return false;

	if (major == "1" && (minor == "0" || minor == "1")) return true;

	return (code = 505, false);
}

State	RequestParser::requestLineParser( Request &request ) {	
	size_t				pos;
	const std::string	CRLF("\r\n");

	if ((pos = request.recv.find(CRLF)) == std::string::npos) {
		if (request.recv.size() > MAX_REQUEST_LINE) return State(414, BAD);
		return State(0, REQUEST_LINE);
	}

	if (pos + CRLF.size() > MAX_REQUEST_LINE) return State(414, BAD);

	std::string			line = request.recv.substr(0, pos);
	int					code;

	request.recv.erase(0, pos + CRLF.size());

	char				sp, space, extra;
	std::istringstream	streamLine(line);

	streamLine >> std::noskipws >> request.method >> sp
		>> request.target >> space >> request.version;

	if (streamLine.fail()) return State(400, BAD);

	if (sp != ' ' || space != ' ') return State(400, BAD);

	if (methodParser(request.method, code) == false) return State(code, BAD);

	if (targetParser(request.target, request.query) == false) return State(400, BAD);

	if (versionParser(request.version, code) == false) return State(code, BAD);

	if (streamLine >> extra) return State(400, BAD);

	return State(0, READING_HEADERS);
}

State	RequestParser::headersParser( Request &request ) {
	std::string			DCRLF("\r\n\r\n");
	size_t				pos;

	if ((pos = request.recv.find(DCRLF)) == std::string::npos) {
		if (request.recv.size() > MAX_HEADER_BYTES) return State(431, BAD);
		return State(0, READING_HEADERS);
	}

	if (pos + DCRLF.size() > MAX_HEADER_BYTES) return State(431, BAD);

	std::stringstream	headersBlock;

	headersBlock << request.recv.substr(0, pos);
	request.recv.erase(0, pos + DCRLF.size());

	std::string			line;
	while (std::getline(headersBlock, line)) {
		if (line.empty()) continue ;

		std::string::size_type	colum = line.find(':');

		if (colum == 0) return State(400, BAD);
		if (colum == std::string::npos) return State(400, BAD);

		std::string				name = line.substr(0, colum);
		std::string				value = line.substr(colum + 1);
		size_t					index;

		for (index = 0; index < name.size(); ++index) {
			if (isTspecials(name[index]) || isCTL(name[index]))
				return State(400, BAD);
			name[index] = std::tolower(name[index]);
		}

		for (index = 0; index < value.size()
			&& containsChar(value[index], " \t"); ++index);
		value.erase(0, index);

		for (index = value.size(); index > 0
			&& containsChar(value[index - 1], "\r \t"); --index)
		value.erase(index - 1);

		for (index = 0; index < value.size(); ++index) {
			if (isCTL(value[index]) && value[index] != '\t')
				return State(400, BAD);
		}

		if (name == "transfer-encoding") return State(400, BAD);
		request.headers[name] = value;
	}

	if (request.method == "POST") return State(0, READING_BODY);
	return State(0, READY_TO_WRITE);
}
