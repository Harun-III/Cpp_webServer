#include "CgiHandler.hpp"

CgiHandler::CgiHandler(const Request& req, const Location& loc)
    : request(req), location(loc) {
}

CgiHandler::~CgiHandler() {
}

