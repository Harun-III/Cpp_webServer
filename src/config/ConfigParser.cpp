#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

# include "Core.hpp"
# include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& filename) 
    : filename(filename) {
}

ConfigParser::~ConfigParser() {
}

std::vector<ServerConfig> ConfigParser::parseConfig() {
    std::vector<ServerConfig> servers;

    if (!readFile()) {
        throwParseError("Could not read configuration file: " + filename);
    }

    currentTokenIndex = 0;

    while (hasMoreTokens()) {
        std::string token = getCurrentToken();
        if (token == "server") {
            servers.push_back(parseServer());
	    incrementTokenIndex();
        } else {
	    throwParseError("Configuration file: " + filename + " is corrupted" );
        }
    }

    if (servers.empty()) {
        throwParseError("No server blocks found in configuration");
    }

    std::cout << GR "[INFO] Loading configuration from "
        << filename << RS << std::endl;

    return servers;
}

ServerConfig ConfigParser::parseServer() {
    ServerConfig server;
    std::map<std::string, bool> unique_listens;

    incrementTokenIndex();
    expectToken("{");

    // loop until '}'
    while (hasMoreTokens() && getCurrentToken() != "}") {
	std::string directive = getCurrentToken();
	if (directive == "listen") {
	    incrementTokenIndex();
	    std::string listen_value = getCurrentToken();
	    std::pair<std::string, std::string> addr = parseListenDirective(listen_value);

	    // NOTE: check if key is duplicated
	    std::string listen_key = addr.first + ":" + addr.second;
	    if (unique_listens.find(listen_key) != unique_listens.end()) {
		throwParseError("Duplicate listen directive: " + listen_key);
	    }
	    unique_listens[listen_key] = true;
	    server.addListen(addr.first, addr.second);
	    incrementTokenIndex();
	    expectToken(";");
	} else if (directive == "error_page") {
	    incrementTokenIndex();
            std::vector<int> error_codes = parseErrorCodes();
            std::string page = getCurrentToken();
	    incrementTokenIndex();
	    expectToken(";");
            for (size_t i = 0; i < error_codes.size(); ++i) {
                server.addErrorPage(error_codes[i], page);
            }
	} else if (directive == "max_client_body_size") {
	    incrementTokenIndex();
            size_t size = static_cast<size_t>(std::atoi(getCurrentToken().c_str()));
            server.setMaxClientBodySize(size);
	    incrementTokenIndex();
	    expectToken(";");
	} else if (directive == "location") {
	    incrementTokenIndex();
	    std::string path = getCurrentToken();
	    incrementTokenIndex();
	    Location location = parseLocation();
	    server.addLocation(path, location);
	} else {
	    throwParseError("Unknown directive in server block: '" + directive + "'");
	}
    }

    if (server.getListen().empty()) {
        throwParseError("Server block must have at least one 'listen' directive");
    }

    if (server.getLocations().empty()) {
        throwParseError("Server block must have at least one 'location' block");
    }

    return server;
}

Location ConfigParser::parseLocation() {
    Location location;

//TODO: maybe make these into a function since I used them alot
    // check for '{'
    std::string token = getCurrentToken();
    if (token != "{") {
	throwParseError("Expected '{' but found '" + token + "'");
    }
    // jump over the '{' token
    incrementTokenIndex();

    while (hasMoreTokens() && getCurrentToken() != "}") {
        std::string directive = getCurrentToken();
        
        if (directive == "methods") {
	    incrementTokenIndex();
	    location.setMethods(parseMethodsList());
	    expectToken(";");
        } else if (directive == "root") {
	    incrementTokenIndex();
            location.setRoot(getCurrentToken());
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "return") {
            std::pair<int, std::string> ret = parseReturnDirective();
	    location.setReturn(ret.first, ret.second);
            expectToken(";");
        } else if (directive == "index") {
	    incrementTokenIndex();
            location.setIndex(getCurrentToken());
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "auto_index") {
	    incrementTokenIndex();
            std::string value = getCurrentToken();
            location.setAutoIndex(value == "on" || value == "yes" || value == "true");
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "upload") {
	    incrementTokenIndex();
            std::string value = getCurrentToken();
            location.setUpload(value == "yes" || value == "on" || value == "true");
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "upload_location") {
	    incrementTokenIndex();
	    location.setUploadLocation(getCurrentToken());
	    incrementTokenIndex();
            expectToken(";");
	} else if (directive == "cgi") {
	    incrementTokenIndex();
	    std::string extension = getCurrentToken();
	    incrementTokenIndex();
	    std::string path = getCurrentToken();
	    location.addCgi(extension, path);
	    incrementTokenIndex();
	    expectToken(";");
	} else {
		throwParseError("Unknown directive in location block: '" + directive + "'");
        }
    }

    expectToken("}");

    if (location.getRoot().empty()) {
	throwParseError("Location block must have a 'root' directive");
    }

    return location;
}
	    
void ConfigParser::expectToken(const std::string& expected) {
    std::string token = getCurrentToken();

    if (token != expected) {
	throwParseError("Expected '" + expected + "' but found '" + token + "'");
    }
    incrementTokenIndex();
}

std::pair<int, std::string> ConfigParser::parseReturnDirective() {
    incrementTokenIndex();
    int status_code = std::atoi(getCurrentToken().c_str());
    if (status_code <= 0 || status_code >= 600) {
	throwParseError("Error code is out of range");
    }
    incrementTokenIndex();
    std::string url = getCurrentToken();
    incrementTokenIndex();
    
    return std::make_pair(status_code, url);
}

std::vector<std::string> ConfigParser::parseMethodsList() {
    std::vector<std::string> methods;

    while (hasMoreTokens() && getCurrentToken() != ";") {
        std::string method = getCurrentToken();
	// remove ',' if there is one
	if (!method.empty() && method[method.size() - 1] == ',') {
	    method = method.substr(0, method.size() - 1);
	}
	if (isValidMethod(method)) {
	    // to upper
	    for (std::string::iterator it = method.begin(); it != method.end(); ++it) {
		*it = std::toupper(static_cast<unsigned char>(*it));
	    }
	    methods.push_back(method);
	}
	incrementTokenIndex();
    }
    return methods;
}

bool ConfigParser::isValidMethod(const std::string& method) {
    return method == "GET" || method == "POST" || method == "DELETE" ||
           method == "get" || method == "post" || method == "delete";
}


std::vector<int> ConfigParser::parseErrorCodes() {
    std::vector<int>	codes;
    bool		firstLoop = true;

    while (hasMoreTokens()) {
        std::string token = getCurrentToken();
	if (!firstLoop) {
	    if (token == ";" || !std::isdigit(token[0])) {
		break;
	    }
	}

	firstLoop = false;

        // Check if entire token is numeric
        bool isNumeric = true;
        for (size_t i = 0; i < token.length(); ++i) {
            if (!std::isdigit(token[i])) {
                isNumeric = false;
                break;
            }
        }
        
        if (!isNumeric) {
            throwParseError("\"" + token +"\"" + " Token is not a valid number");
        }
        
        int code = std::atoi(token.c_str());
        
        if (code >= 400 && code < 600) {
            codes.push_back(code);
        } else {
            throwParseError("Error code is out of range (must be 4xx or 5xx)");
        }
        incrementTokenIndex();
    }
    
    return codes;
}

std::pair<std::string, std::string> ConfigParser::parseListenDirective(const std::string& value) {
    size_t colon_pos = value.find(':');

    if (colon_pos == std::string::npos) {
        throwParseError("Invalid listen directive format. Expected ip:port");
    }
    
    std::string ip = value.substr(0, colon_pos);
    std::string port_str = value.substr(colon_pos + 1);

    int port = std::atoi(port_str.c_str());
    if (port <= 0 || port > 65535) {
        throwParseError("Invalid port number: " + port_str);
    }

    std::stringstream   ss;

    ss << port_str;

    return std::make_pair(ip, port_str.c_str());
}


void ConfigParser::incrementTokenIndex() {
    if (!hasMoreTokens()) {
        throwParseError("Unexpected end of file");
    }
    currentTokenIndex++;
}

std::string ConfigParser::getCurrentToken() {
    if (!hasMoreTokens()) {
        throwParseError("Unexpected end of file");
    }
    return tokens[currentTokenIndex];
}

bool ConfigParser::hasMoreTokens() {
   return currentTokenIndex < tokens.size();
}

bool ConfigParser::readFile() {
    std::ifstream file(filename.c_str());

    if (!file.is_open()) {
	return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

    file.close();
    tokenize(content);
    return true;
}

void ConfigParser::tokenize(const std::string& content) {
    bool		in_comment = false;
    std::stringstream	ss;

    // loop over the whole string
    for (size_t i = 0; i < content.length(); ++i) {
	char c = content[i];

	if (c == '#' && !in_comment) {
	    in_comment = true;
	    continue;
	}

	if (c == '\n') {
	    ss << " ";
	    in_comment = false;
	    continue;
	}

	if (in_comment) {
	    continue;
	}

	if (c == '{' || c == '}' || c == ';') {
	    if (!ss.str().empty()) {
		std::string token = ss.str();
		trim(token);
		if (!token.empty()) {
		    tokens.push_back(token);
		}
		ss.str("");
	    }
	    tokens.push_back(std::string(1, c));
	} else if (std::isspace(c)) {
	    std::string token = ss.str();
	    trim(token);
	    if (!token.empty()) {
		tokens.push_back(token);
	    }
	    ss.str("");
	} else {
	    ss << c;
	}
    }

    if (!ss.str().empty()) {
	std::string token = ss.str();
	trim(token);
	if (!token.empty()) {
	    tokens.push_back(token);
	}
    }
}

void ConfigParser::trim(std::string& s) {
    while (!s.empty() && std::isspace(s[0])) {
        s.erase(s.begin());
    }
    while (!s.empty() && std::isspace(s[s.size() - 1])) {
        s.erase(s.end() - 1);
    }
}

void ConfigParser::throwParseError(const std::string& message) {
    throw std::runtime_error("Parse error: " + message);
}
