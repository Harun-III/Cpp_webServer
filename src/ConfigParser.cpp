#include "../includes/ConfigParser.hpp"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

ConfigParser::ConfigParser(const std::string& filename) 
    : filename(filename) {
}

ConfigParser::~ConfigParser() {
}

ServerConfig ConfigParser::parseConfig() {
    ServerConfig    server;

    if (!readFile()) {
        throwParseError("Could not read configuration file: " + filename);
    }

    // loop over tokens and do logic
    
    
    return server;
}

bool ConfigParser::readFile() {
    std::ifstream file(filename);

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
size_t i = 0;
for (auto it_b = tokens.begin(); it_b != tokens.end(); it_b++) {
    std::cout << ++i << " : " << *it_b << std::endl;
}
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
    while (!s.empty() && std::isspace(s.front()) ) {
        s.erase(s.begin());
    }
    //trailing white-space
    while (!s.empty() && std::isspace(s.back()) ) {
        s.pop_back();
    }
}


void ConfigParser::throwParseError(const std::string& message) {
    throw std::runtime_error("Parse error: " + message);
}

