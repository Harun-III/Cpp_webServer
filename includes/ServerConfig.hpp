#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Location.hpp"
#include <cstddef>
#include <map>
#include <vector>
#include <string>

class ServerConfig {
private:
    std::vector<std::pair<std::string, int> >   listen; // ip:port pairs
    std::map<std::string, Location>             locations;
    std::map<int, std::string>                  error_pages;
    size_t                                      max_client_body_size;

public:
    ServerConfig();
    ~ServerConfig();

    void            addErrorPage(int error_code, const std::string& page);


    // setters
    void            addListen(const std::string& ip, int port);
    void            setMaxClientBodySize(size_t size);

    // getters
    size_t          getMaxClientBodySize() const;

};

#endif

