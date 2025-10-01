#include "../includes/StaticFileHandler.hpp"

StaticFileHandler::StaticFileHandler() {
}

StaticFileHandler::~StaticFileHandler() {
}

bool StaticFileHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}


