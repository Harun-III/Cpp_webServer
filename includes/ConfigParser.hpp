#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ServerConfig.hpp"
#include <cstddef>
#include <string>
#include <vector>

class ConfigParser {
private:
    std::string                 filename;
    std::vector<std::string>    tokens;
    size_t                      currentTokenIndex;

public:
    ConfigParser(const std::string& filename);
    ~ConfigParser();

    ServerConfig parseConfig();

private:
    bool                            readFile();
    void                            tokenize(const std::string& content);

    ServerConfig                    parseServer();

    std::vector<int>                parseErrorCodes();

    std::pair<std::string, int>     parseListenDirective(const std::string& value);

    void                            incrementTokenIndex();
    std::string                     getCurrentToken();
    bool                            hasMoreTokens();

    void                            trim(std::string& str);
    void                            throwParseError(const std::string& message);
};

#endif

