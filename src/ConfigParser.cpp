#include "../includes/ConfigParser.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

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
    std::cout << "hello I'm the tokenizer" << std::endl;
    std::cout << "And this is the file: " << std::endl;
    for (auto it_b = content.begin(); it_b != content.end(); it_b++) {
	std::cout << *it_b;
    }
/****************************************************************************/
/*
    1_ get rid of comments
    e.g.:
	# this is a comment, comments end with \n
*/

// loop over the whole string
bool in_comment = false;
std::stringstream ss;

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

    ss << c;
    
    }

/*********************************TESTS BLOCK********************************/
std::cout << "\nAnd this should be the file wihtout comments nor \'\\n\'" << std::endl;
std::string test = ss.str();
for (auto it_b = test.begin(); it_b != test.end(); it_b++) {
    std::cout << *it_b;
}
/****************************************************************************/

}

void ConfigParser::throwParseError(const std::string& message) {
    throw std::runtime_error("Parse error: " + message);
}

