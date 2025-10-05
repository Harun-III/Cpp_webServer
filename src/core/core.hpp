# pragma once

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

// ------------------------- Standard library -------------------------//

# include <vector>
# include <iostream>
# include <fstream>
# include <iterator>
# include <cstdlib>
# include <ctype.h>
# include <limits.h>
# include <climits>
# include <cstring>
# include <bitset>
# include <algorithm>
# include <sstream>

// ------------------------- Webserv includes -------------------------//

# include "ServerConfig.hpp"
# include "ConfigParser.hpp"
# include "Server.hpp"

// ------------------------- Webserv Macros -------------------------//

# define BUF_SIZE 10
# define PORT_NUM 50002
# define MAX_EVENTS 1024
# define TIMEOUT 1000

# define ERROR -1

// ------------------------- Webserv Outils -------------------------//

void	printServers( const std::vector<ServerConfig> &servers );
