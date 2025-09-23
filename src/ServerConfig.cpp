#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig() : max_client_body_size(10100100) {
}

ServerConfig::~ServerConfig() {
}

void ServerConfig::addErrorPage(int error_code, const std::string& page) {
    error_pages[error_code] = page;
}

// setters
void ServerConfig::addListen(const std::string& ip, int port) {
    listen.push_back(std::make_pair(ip, port));
}

void ServerConfig::setMaxClientBodySize(size_t size) {
    this->max_client_body_size = size;
}

// getters
size_t ServerConfig::getMaxClientBodySize() const {
    return max_client_body_size;
}

