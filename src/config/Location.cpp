#include "Location.hpp"

Location::Location() {
}

Location::~Location() {
}

// Setters
void Location::setMethods(const std::vector<std::string>& methods) {
    this->methods = methods;
}

void Location::setRoot(const std::string& root) {
    this->root = root;
}

void Location::setUploadLocation(const std::string& location) {
    this->upload_location = location;
}

void Location::setAutoIndex(bool auto_index) {
    this->auto_index = auto_index;
}

void Location::setUpload(bool upload) {
    this->upload = upload;
}

void Location::setIndex(const std::string& index) {
    this->index = index;
}

void Location::setReturn(int status, const std::string& url) {
    return_directive = std::make_pair(status, url);
}

void Location::addCgi(const std::string& extension, const std::string& path) {
    cgi[extension] = path;
}

// Getters
const std::vector<std::string>& Location::getMethods() const {
    return methods;
}

const std::string&	Location::getRoot() const {
    return root;
}

const std::string&	Location::getUploadLocation() const {
    return upload_location;
}

bool			Location::getAutoIndex() const {
    return auto_index;
}

bool			Location::getUpload() const {
    return upload;
}

const std::string&	Location::getIndex() const {
    return index;
}

const std::pair<int, std::string>& Location::getReturn() const {
    return return_directive;
}

const std::map<std::string, std::string>& Location::getCgi() const {
    return cgi;
}

