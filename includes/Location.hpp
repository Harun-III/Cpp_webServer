#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <vector>
#include <string>
#include <map>

class Location {
private:
    std::vector<std::string> methods;
    std::string root;
    std::pair<int, std::string> return_directive; // status code, redirect URL
    std::string index;
    bool auto_index;
    bool upload;
    std::string upload_location;
    std::map<std::string, std::string> cgi; // extension -> cgi path

public:
    Location();
    ~Location();
};

#endif

