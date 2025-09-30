#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "../includes/HttpResponse.hpp"
#include "Location.hpp"

class Client {
private:

public:
    int          client_fd;      // Client socket file descriptor
    std::string  ip;             // Client IP address
    int          port;           // Client port number
    std::string  res_buffer;     // Incoming data buffer (partial requests)
    std::string  snd_buffer;     // Outgoing data buffer
    Location        location;
    // ClientState  state;          // Current processing state (READING/PROCESSING/SENDING/FINISHED)
    // HttpRequest  request;
    HttpResponse response;
};

#endif


