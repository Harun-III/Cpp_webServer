#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "../includes/HttpResponse.hpp"
#include "../includes/HttpRequest.hpp"
#include "../includes/StaticFileHandler.hpp"
#include "Location.hpp"

class Client {
private:

public:

    //NOTE: location should be under "HttpRequest"
    // I'v put it here since I don't have access to request yet
        Location        location;
    HttpRequest  request;

    int          client_fd;      // Client socket file descriptor
    std::string  ip;             // Client IP address
    int          port;           // Client port number
    std::string  res_buffer;     // Incoming data buffer (partial requests)
    std::string  snd_buffer;     // Outgoing data buffer
    // ClientState  state;          // Current processing state (READING/PROCESSING/SENDING/FINISHED)
    // HttpRequest  request;
    HttpResponse response;
};

#endif


