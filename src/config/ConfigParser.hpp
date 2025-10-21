
#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Core.hpp"
# include "ServerConfig.hpp"

class ConfigParser {
private:
    std::string                 filename;
    std::vector<std::string>    tokens;
    size_t                      currentTokenIndex;

public:
    ConfigParser(const std::string& filename);
    ~ConfigParser();

    std::vector<ServerConfig> parseConfig();

private:
    bool                            readFile();
    void                            tokenize(const std::string& content);

    ServerConfig                    parseServer();
    Location                        parseLocation();
    std::vector<std::string>        parseMethodsList();
    std::vector<int>                parseErrorCodes();
    std::pair<std::string, std::string> parseListenDirective(const std::string& value);
    std::pair<int, std::string>     parseReturnDirective();

    void                            incrementTokenIndex();
    std::string                     getCurrentToken();
    bool                            hasMoreTokens();
    void                            expectToken(const std::string& expected);

    bool                            isValidMethod(const std::string& method);

    void                            trim(std::string& str);
    void                            throwParseError(const std::string& message);
};

#endif
