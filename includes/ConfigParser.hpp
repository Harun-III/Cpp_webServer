#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ServerConfig.hpp"
#include <string>

class ConfigParser {
private:
    std::string filename;

public:
    ConfigParser(const std::string& filename);
    ~ConfigParser();

    ServerConfig parseConfig();

private:
    bool readFile();
    void tokenize(const std::string& content);

    void throwParseError(const std::string& message);
};

#endif

