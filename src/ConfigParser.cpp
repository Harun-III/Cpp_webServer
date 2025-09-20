#include "../includes/ConfigParser.hpp"
#include <fstream>
#include <iostream>
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

// here we should tokenize the content

    return true;
}


void ConfigParser::throwParseError(const std::string& message) {
    throw std::runtime_error("Parse error: " + message);
}

