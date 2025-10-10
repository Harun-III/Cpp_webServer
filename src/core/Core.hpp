#ifndef CORE_HPP
# define CORE_HPP

// ------------------------- System includes -------------------------//

# include <sys/types.h>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>
# include <strings.h>
# include <iomanip>
# include <algorithm>
# include <cctype>

// ------------------------- Standard library -------------------------//

# include <vector>
# include <iostream>
# include <iterator>
# include <cstdlib>
# include <ctype.h>
# include <limits.h>
# include <climits>
# include <cstring>
# include <bitset>
# include <algorithm>
# include <fstream>
# include <sstream>
# include <map>

// ------------------------- Webserv includes -------------------------//

# include "ServerConfig.hpp"
# include "ConfigParser.hpp"
# include "Server.hpp"

// ------------------------- Webserv Typedefs -------------------------//

typedef std::vector< std::pair<std::string, std::string> > vector_pairs;

// ------------------------- Webserv Macros -------------------------//

# define BUF_SIZE 1024
# define MAX_EVENTS 1024

# define TIMEOUT 1000
# define ERROR -1

# define MAX_REQUEST_LINE 1024
# define MAX_HEADER_BYTES 10240
# define MAX_HEADER_COUNT 100

// ------------------------- Webserv Outils -------------------------//

void	printServers( const std::vector<ServerConfig> &servers );

#endif
