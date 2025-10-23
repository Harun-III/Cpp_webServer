
struct addrinfo <ai>;
    struct sockaddr {};
- call: getaddrinfo(node, service, &hints, &res)  
    • res → first element of linked list of possible addrinfos  
    • node=NULL + AI_PASSIVE → bind to all interfaces (0.0.0.0 / ::)  
    • node=NULL w/o AI_PASSIVE → localhost (127.0.0.1 / ::1)  
    • service="8080" → port number (or service name like "http")
    e.g.:
	int getaddrinfo(NULL, "3490", &hints, &servinfo);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);  
    struct sockaddr_in { // for IPv4  };  
    struct in_addr {  };  
    struct sockaddr_in6 { // for IPv6  };  
    struct in6_addr {  };

==========================================================================

#include <sys/types.h>  
#include <sys/socket.h>  
#include <netdb.h>  

int getaddrinfo(  
    const char *node,   // e.g. "www.example.com" or IP
    const char *service,// e.g. "http" or port number
    const struct addrinfo *hints,
    struct addrinfo **res
);

==========================================================================

int status;  
struct addrinfo hints;  
struct addrinfo *servinfo; // will point to the results  

memset(&hints, 0, sizeof hints);  
// make sure the struct is empty  
hints.ai_family = AF_UNSPEC;  
// don't care IPv4 or IPv6  
hints.ai_socktype = SOCK_STREAM;  
// TCP stream sockets  
hints.ai_flags = AI_PASSIVE;  
// fill in my IP for me  
if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {  
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));  
    exit(1);  
}  
// servinfo now points to a linked list of 1 or more struct addrinfos  
// ... use servinfo ...  
freeaddrinfo(servinfo);  
// free the linked-list 

==========================================================================

struct addrinfo {  
    int ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.  
    int ai_family;    // AF_INET, AF_INET6, AF_UNSPEC  
    int ai_socktype; // SOCK_STREAM, SOCK_DGRAM  
    int ai_protocol; // use 0 for "any"  
    size_t ai_addrlen; // size of ai_addr in bytes  
    struct sockaddr *ai_addr; // struct sockaddr_in or _in6  
    char *ai_canonname; // full canonical hostname  
    struct addrinfo *ai_next; // linked list, next node  
}  

- prepare struct addrinfo hints → tells what kind of socket we want  
- call: getaddrinfo(node, service, &hints, &res)  

• res → first element of linked list of possible addrinfos  
• node=NULL + AI_PASSIVE → bind to all interfaces (0.0.0.0 / ::)  
• node=NULL w/o AI_PASSIVE → localhost (127.0.0.1 / ::1)  
• service="8080" → port number (or service name like "http")

==========================================================================

struct sockaddr {  
    unsigned short sa_family; // AF_INET (IPv4) or AF_INET6 (IPv6)
	char sa_data[14]; // contains a destination address and port number for the socket 
};

==========================================================================
functions like bind() expect a struct sockaddr  

e.g.:  
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);  

so what we do to not fill sa_data by hand is use two different structures:  

struct sockaddr_in { // for IPv4  
    short int sin_family;   // Address family, AF_INET  
    unsigned short int sin_port; // Port number  
    struct in_addr sin_addr; // Internet address  
    unsigned char sin_zero[8]; // Same size as struct sockaddr  
};  

struct in_addr {  
    uint32_t s_addr; // that's a 32-bit int (4 bytes)  
};  

and:  

struct sockaddr_in6 { // for IPv6  
    u_int16_t sin6_family;   // address family, AF_INET6  
    u_int16_t sin6_port;     // port, Network Byte Order  
    u_int32_t sin6_flowinfo; // IPv6 flow information  
    struct in6_addr sin6_addr; // IPv6 address  
    u_int32_t sin6_scope_id; // Scope ID  
};  

struct in6_addr {  
    unsigned char s6_addr[16]; // IPv6 address  
};

==========================================================================

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
#
sockfd: socket file descriptor
serv_addr: struct sockaddr containing the destination and IP address
addrlen: length in bytes of the server address structure

==========================================================================

int listen(int sockfd, int backlog);
sockfd: socket file descriptor.
backlog is the number of connections allowed on the incoming queue. 
Incoming connections are going to wait in this queue until you accept() them
(see below) and this is the limit on how many can queue up.
[[NOTE: Most systems silently limit this number to about 20; you can probably get away
with setting it to 5 or 10.]]

==========================================================================

#include <sys/types.h>
#include <sys/socket.h>
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
sockfd: socket file descriptor
*addr: points to struct sockaddr_storage that containes iformation about the
    incoming connection (IP, Port, ...)
addrlen: is a local integer variable
    that should be set to sizeof(struct sockaddr_storage)
    before its address is passed to accept().
NOTE: accept() will not put more than that many bytes into addr.
    If it puts fewer in, it’ll change the value of addrlen to reflect that.

==========================================================================

int send(int sockfd, const void *msg, int len, int flags);
Example:
char *msg = "Message to send";
int len, bytes_sent;
#
len = strlen(msg);
bytes_sent = send(sockfd, msg, len, 0);


