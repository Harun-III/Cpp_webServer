#ifndef STATICFILEHANDLER_HPP
#define STATICFILEHANDLER_HPP

#include <sys/stat.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <dirent.h>
#include <sstream>
#include <fstream>


class StaticFileHandler {
private:
    std::string                 getMimeType(const std::string& path) const;
    bool                        isPathSafe(const std::string& path) const;

public:
    StaticFileHandler();
    ~StaticFileHandler();
    
    bool                        fileExists(const std::string& path) const;
    bool                        isDirectory(const std::string& path) const;
    bool                        isReadable(const std::string& path) const;
    std::string                 readFile(const std::string& path) const;
    bool                        deleteFile(const std::string& path) const;

    std::vector<std::string>    listDirectory(const std::string& path) const;
    std::string                 getContentType(const std::string& path) const;


};

#endif

