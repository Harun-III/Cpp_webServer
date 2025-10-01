#ifndef STATICFILEHANDLER_HPP
#define STATICFILEHANDLER_HPP

#include <sys/stat.h>
#include <string>

class StaticFileHandler {
private:
    
public:
    StaticFileHandler();
    ~StaticFileHandler();
    
    // Check if file exists
    bool fileExists(const std::string& path) const;

};

#endif

