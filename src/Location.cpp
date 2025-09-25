#include "../includes/Location.hpp"

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


// Getters
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

