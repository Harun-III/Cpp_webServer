#include "ConfigParser.hpp"
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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
        } else {
	    incrementTokenIndex();
        }
    }

    if (servers.empty()) {
        throwParseError("No server blocks found in configuration");
    }

    return servers;
}

ServerConfig ConfigParser::parseServer() {
    ServerConfig server;

/*********************************TESTS BLOCK********************************/
    // std::cout << "inside parseServer" << std::endl;
/****************************************************************************/

    // jump over the "server" token
    incrementTokenIndex();
    
    // Expect opening brace
    if (getCurrentToken() != "{") {
        throwParseError("Expected '{' after 'server'");
    }

    // jump over the "{" token
    incrementTokenIndex();

    // loop until '}'
    while (hasMoreTokens() && getCurrentToken() != "}") {
/*
    std::vector<std::pair<std::string, int> >	listen; // ip:port pairs
    std::map<std::string, Location>		locations;
    std::map<int, std::string>			error_pages;
    size_t					max_client_body_size;

    Directives:
	listen 
	error_page
	max_client_body_size
	location
*/
	std::string directive = getCurrentToken();

	if (directive == "listen") {
	    //'listen' '127.0.0.1:8080' ';'
	    incrementTokenIndex();
	    //'127.0.0.1:8080' ';'
	    std::string listen_value = getCurrentToken();
	    std::pair<std::string, std::string> addr = parseListenDirective(listen_value);
	    server.addListen(addr.first, addr.second);
	    incrementTokenIndex();
	    // --> we should technically have ';'
	    // So we should check for it
	    std::string token = getCurrentToken();
	    if (token != ";") {
		throwParseError("Expected ';' but found '" + token + "'");
	    }
	    // jump over the ';' token
	    incrementTokenIndex();
	} else if (directive == "error_page") {
//   # Error pages
//     error_page 404 405 /errors/404.html;
//     error_page 403 /errors/403.html;
	    incrementTokenIndex();
            std::vector<int> error_codes = parseErrorCodes();
            std::string page = getCurrentToken();
	    incrementTokenIndex();

	    // check for ';'
	    std::string token = getCurrentToken();
	    if (token != ";") {
		throwParseError("Expected ';' but found '" + token + "'");
	    }
	    // jump over the ';' token
	    incrementTokenIndex();
            
            for (size_t i = 0; i < error_codes.size(); ++i) {
                server.addErrorPage(error_codes[i], page);
            }
	} else if (directive == "max_client_body_size") {
	    incrementTokenIndex();
            size_t size = static_cast<size_t>(std::atoi(getCurrentToken().c_str()));
            server.setMaxClientBodySize(size);

	    incrementTokenIndex();
	    // check for ';'
	    std::string token = getCurrentToken();
	    if (token != ";") {
		throwParseError("Expected ';' but found '" + token + "'");
	    }
	    // jump over the ';' token
	    incrementTokenIndex();
    // do logic
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
/*********************************TESTS BLOCK********************************/
// std::cout << "server getMaxClientBodySize " << server.getMaxClientBodySize() << std::endl;
/****************************************************************************/

    // Validate that server has at least one listen directive
    if (server.getListen().empty()) {
        throwParseError("Server block must have at least one 'listen' directive");
    }

    // Validate that server has at least one location
    if (server.getLocations().empty()) {
        throwParseError("Server block must have at least one 'location' block");
    }

    return server;
}

Location ConfigParser::parseLocation() {
/*********************************TESTS BLOCK********************************/
// std::cout << "tokens are: \n" << std::endl;
// for (auto b = tokens.begin(); b != tokens.end(); b++) {
//     std::cout << "\"" << *b << "\" ";
// }
// std::cout << std::endl;
/****************************************************************************/
    Location location;

//TODO: maybe make these into a function since I used them alot
    // check for '{'
    std::string token = getCurrentToken();
    if (token != "{") {
	throwParseError("Expected '{' but found '" + token + "'");
    }
    // jump over the '{' token
    incrementTokenIndex();

/* NOTE: location directives: 
    +std::vector<std::string>              methods; // Allowed methods (GET, POST, DELETE)
    +std::string                           root; // Root directory for this location
    std::pair<int, std::string>           return_directive; // status code, redirect URL
    std::string                           index; // Default file to serve (index.html)
    bool                                  auto_index; // Enable/disable directory listing
    bool                                  upload; // Allow file uploads
    +std::string                           upload_location; // Directory to store uploaded files
    std::map<std::string, std::string>    cgi; // extension -> cgi path
*/

    while (hasMoreTokens() && getCurrentToken() != "}") {
        std::string directive = getCurrentToken();
        
        if (directive == "methods") {
	    incrementTokenIndex();
/*********************************TESTS BLOCK********************************/
// NOTE: if you use this comment the second usage of the function parseMethodsList() below
	    
   // std::vector<std::string> string_of_allowed_methods = parseMethodsList();
   // std::cout << "Block locaiton: mthods are: " << std::endl;
   // for (size_t i = 0; i < string_of_allowed_methods.size(); i++) {
   //   std::cout << i + 1 << ": " << string_of_allowed_methods[i] << " ";
   // }
   // std::cout << std::endl;
/****************************************************************************/
	    location.setMethods(parseMethodsList());
	    expectToken(";");
        } else if (directive == "root") {
	    incrementTokenIndex();
            location.setRoot(getCurrentToken());
/*********************************TESTS BLOCK********************************/
// std::cout << "location.root == " << location.getRoot() << std::endl;
/****************************************************************************/
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "return") {
            std::pair<int, std::string> ret = parseReturnDirective();
/*********************************TESTS BLOCK********************************/
// std::cout << "Return: \n" << "\tcode: " << ret.first <<
// 		"\n\tPath: " << ret.second << location.getRoot() << std::endl;
/****************************************************************************/
	    location.setReturn(ret.first, ret.second);
/*********************************TESTS BLOCK********************************/
// std::pair<int, std::string> test_ret = location.getReturn();
// std::cout << "code: " << test_ret.first << std::endl;
// std::cout << "Path: " << test_ret.second << std::endl;
/****************************************************************************/
            expectToken(";");
        } else if (directive == "index") {
	    incrementTokenIndex();
            location.setIndex(getCurrentToken());
/*********************************TESTS BLOCK********************************/
// std::cout << "location.index == " << location.getIndex() << std::endl;
/****************************************************************************/
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "auto_index") {
	    incrementTokenIndex();
            std::string value = getCurrentToken();
            location.setAutoIndex(value == "on" || value == "yes" || value == "true");
/*********************************TESTS BLOCK********************************/
//   std::cout << "location.auto_index == " << 
// (location.getAutoIndex() ? "yes" : "no") << std::endl;
/****************************************************************************/
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "upload") {
	    incrementTokenIndex();
            std::string value = getCurrentToken();
            location.setUpload(value == "yes" || value == "on" || value == "true");
/*********************************TESTS BLOCK********************************/
//   std::cout << "location.Upload == " << 
// (location.getUpload() ? "yes" : "no") << std::endl;
/****************************************************************************/
	    incrementTokenIndex();
            expectToken(";");
        } else if (directive == "upload_location") {
	    incrementTokenIndex();
	    location.setUploadLocation(getCurrentToken());
/*********************************TESTS BLOCK********************************/
// std::cout << "location.upload_location == " << location.getUploadLocation() << std::endl;
/****************************************************************************/
	    incrementTokenIndex();
            expectToken(";");
	} else if (directive == "cgi") {
	    incrementTokenIndex();
	    std::string extension = getCurrentToken();
	    incrementTokenIndex();
	    std::string path = getCurrentToken();
	    location.addCgi(extension, path);
/*********************************TESTS BLOCK********************************/
// std::map<std::string, std::string>::const_iterator it = location.getCgi().begin();
// for (; it != location.getCgi().end() ; it++) {
//     std::cout << it->first << ": " << it->second << std::endl;
// }
/****************************************************************************/
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
    //NOTE: e.g.: "POST," "DELETE" "," "get" "GET" ";"
    std::vector<std::string> methods;

    while (hasMoreTokens() && getCurrentToken() != ";") {
        std::string method = getCurrentToken();
	// remove ',' if there is one
	if (!method.empty() && method[method.size() - 1] == ',') {
	    method = method.substr(0, method.size() - 1);
	}
	if (isValidMethod(method)) {
	    methods.push_back(method);
	}
	
	incrementTokenIndex();
    }
    return methods;
}

bool ConfigParser::isValidMethod(const std::string& method) {
    //NOTE: valid method: GET, POST, DELETE
    return method == "GET" || method == "POST" || method == "DELETE" ||
           method == "get" || method == "post" || method == "delete";

}


std::vector<int> ConfigParser::parseErrorCodes() {
    std::vector<int> codes;
    
    bool firstLoop = true;
    while (hasMoreTokens()) {
        std::string token = getCurrentToken();
/*********************************TESTS BLOCK********************************/
// std::cout << "token --> " << token << std::endl;
/****************************************************************************/
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
            throwParseError("Error code is out of range (must be 4xx)");
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
/*********************************TESTS BLOCK********************************/
    //    std::cout << "hello I'm the tokenizer" << std::endl;
    //    std::cout << "And this is the file: " << std::endl;
    //    for (auto it_b = content.begin(); it_b != content.end(); it_b++) {
    // std::cout << *it_b;
    //    }
/****************************************************************************/
/*
    e.g.:
	# this is a comment, testing
	server {
	    port 8080;
	}

    1_ get rid of comments and '\n'
    e.g.:
	# this is a comment, comments end with \n

    So you will either encounter a white-space
	a normal char of a special token ( "{" , "}" , ";")
    2_ Special character tokens:
	Suppose your config file contains:
	e.g.:
	server {
	    port 8080;
	}
*/

    bool in_comment = false;
    std::stringstream ss;

    // loop over the whole string
    for (size_t i = 0; i < content.length(); ++i) {
	char c = content[i];

	// if we hit # set flag to true, this means we are in a comment
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

	//       8080;
	//	     ^
	if (c == '{' || c == '}' || c == ';') {
	    //ss="       8080" && c = ';'
	    if (!ss.str().empty()) {
		std::string token = ss.str();
		trim(token);
		// token="8080"
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
/*********************************TESTS BLOCK********************************/
// size_t i = 0;
// for (auto it_b = tokens.begin(); it_b != tokens.end(); it_b++) {
//     std::cout << ++i << " : " << *it_b << std::endl;
// }
/****************************************************************************/

/*********************************TESTS BLOCK********************************/
// std::cout << "\nAnd this should be the file wihtout comments nor \'\\n\'" << std::endl;
// std::string test = ss.str();
// for (auto it_b = test.begin(); it_b != test.end(); it_b++) {
//     std::cout << *it_b;
// }
/****************************************************************************/
}

void ConfigParser::trim(std::string& s) {
    // leading white-space
    while (!s.empty() && std::isspace(s[0])) {
        s.erase(s.begin());
    }
    // trailing white-space
    while (!s.empty() && std::isspace(s[s.size() - 1])) {
        s.erase(s.end() - 1);
    }
}

void ConfigParser::throwParseError(const std::string& message) {
    throw std::runtime_error("Parse error: " + message);
}
