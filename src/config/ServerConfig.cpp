# include "ServerConfig.hpp"

ServerConfig::ServerConfig() : max_client_body_size(10100100) {
}

ServerConfig::~ServerConfig() {
}

void ServerConfig::addErrorPage(int error_code, const std::string& page) {
    error_pages[error_code] = page;
}

// setters
void ServerConfig::addListen(const std::string& ip, const std::string& port) {
    listen.push_back(std::make_pair(ip, port));
}

void ServerConfig::setMaxClientBodySize(size_t size) {
    this->max_client_body_size = size;
}

void ServerConfig::setIpPort(const std::string& ip, const std::string& port) {
    this->ip = ip;
    this->port = port;
}

std::string ServerConfig::getIp() const {
    return ip;
}

std::string ServerConfig::getPort() const {
    return port;
}

void ServerConfig::addLocation(const std::string& path, const Location& location) {
    locations[path] = location;
}

// getters
const std::vector<std::pair<std::string, std::string> >& ServerConfig::getListen() const {
    return listen;
}

const std::map<std::string, Location>& ServerConfig::getLocations() const {
    return locations;
}

const std::map<int, std::string>& ServerConfig::getErrorPages() const {
    return error_pages;
}

size_t ServerConfig::getMaxClientBodySize() const {
    return max_client_body_size;
}
