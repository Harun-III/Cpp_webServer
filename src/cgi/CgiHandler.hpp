#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Request.hpp"
#include "Location.hpp"

class CgiHandler {
private:
	const Request&      request;
	const Location&     location;

public:
    CgiHandler(const Request& req, const Location& loc);
    ~CgiHandler();

};

#endif

