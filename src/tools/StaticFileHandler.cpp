#include <algorithm>
#include <dirent.h>
#include <cstdio>
#include "StaticFileHandler.hpp"

StaticFileHandler::StaticFileHandler() {
}

StaticFileHandler::~StaticFileHandler() {
}

std::string StaticFileHandler::getMimeType(const std::string& path) const {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "application/octet-stream";
    }
    
    std::string extension = path.substr(dot_pos + 1);
    
    // Convert to lowercase
    for (std::string::iterator it = extension.begin(); it != extension.end(); ++it) {
        *it = std::tolower(static_cast<unsigned char>(*it));
    }
    
    if (extension == "txt")  return "text/plain";
    if (extension == "html") return "text/html";

    return "application/octet-stream";
}

bool StaticFileHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool StaticFileHandler::isDirectory(const std::string& path) const {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

bool StaticFileHandler::isReadable(const std::string& path) const {
    return (access(path.c_str(), R_OK) == 0);
}

std::string StaticFileHandler::readFile(const std::string& path) const {
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

bool    StaticFileHandler::deleteFile(const std::string& path) const {
    return (std::remove(path.c_str()) == 0);
}

std::vector<std::string>    StaticFileHandler::listDirectory(const std::string& path) const {
    std::vector<std::string> entries;
   
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        return entries;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;

        // Skip . and ..
        if (name == "." || name == "..") {
            continue;
        }

        // Add "/" to directories
        std::string full_path = path + "/" + name;
        if (isDirectory(full_path)) {
            name += "/";
        }

        entries.push_back(name);
    }

    closedir(dir);
    
    std::sort(entries.begin(), entries.end());

    return entries;
}

std::string StaticFileHandler::getContentType(const std::string& path) const {
    return getMimeType(path);
}

bool StaticFileHandler::isPathSafe(const std::string& path) const {
    // Check for directory traversal attempts
    if (path.find("..") != std::string::npos) {
        return false;
    }
    
    // Check for null bytes
    if (path.find('\0') != std::string::npos) {
        return false;
    }
    
    return true;
}
