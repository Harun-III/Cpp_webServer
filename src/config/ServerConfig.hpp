#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Location.hpp"
#include <cstddef>
#include <map>
#include <vector>
#include <string>

class ServerConfig {
private:
    std::vector<std::pair<std::string, std::string> >   listen; // ip:port pairs
    std::map<std::string, Location>                     locations;
    std::map<int, std::string>                          error_pages;
    size_t                                              max_client_body_size;

public:
    ServerConfig();
    ~ServerConfig();

    void            addErrorPage(int error_code, const std::string& page);

    // setters
    void            addListen(const std::string& ip, const std::string& port);
    void            setMaxClientBodySize(size_t size);
    void            addLocation(const std::string& path, const Location& location);

    // getters
    const std::vector<std::pair<std::string, std::string> >&    getListen() const;
    const std::map<int, std::string>&                   getErrorPages() const;
    const std::map<std::string, Location>&              getLocations() const;
    size_t          getMaxClientBodySize() const;

};

#endif
