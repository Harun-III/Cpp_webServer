#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig() : max_client_body_size(10100100) {
}

ServerConfig::~ServerConfig() {
}

// setters
void ServerConfig::addListen(const std::string& ip, int port) {
    listen.push_back(std::make_pair(ip, port));
}

