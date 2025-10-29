#ifndef CORE_HPP
# define CORE_HPP

// ------------------------- System includes -------------------------//

# include <ctime>
# include <cctype>
# include <cstddef>
# include <netdb.h>
# include <fcntl.h>
# include <unistd.h>
# include <strings.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/epoll.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>

// ------------------------- Standard library -------------------------//

# include <map>
# include <cstdio>
# include <vector>
# include <cerrno>
# include <bitset>
# include <cstdlib>
# include <ctype.h>
# include <fstream>
# include <csignal>
# include <sstream>
# include <climits>
# include <cstdlib>
# include <cstring>
# include <iomanip>
# include <iostream>
# include <dirent.h>
# include <iterator>
# include <limits.h>
# include <algorithm>

// ------------------------- Webserv Typedefs -------------------------//

typedef std::map<std::string, std::string>					map_s;
typedef struct addrinfo										addrinfo_t;
typedef std::vector< std::pair<std::string, std::string> >	vector_pairs;

// ------------------------- Webserv Macros -------------------------//

# define ERROR				-1
# define BUF_SIZE			(32 * 1024)
# define MAX_EVENTS			1024

# ifndef TIMEOUT
#  define TIMEOUT 10
# endif

# define MAX_REQUEST_LINE	1024
# define MAX_HEADER_BYTES	(10 * 1024)

# define GR					"\033[1;32m"
# define YL					"\033[1;33m"
# define RD					"\033[1;31m"
# define RS					"\033[0m"

// ------------------------- Webserv Enum -------------------------//

enum cgi_e {
	CGI_INIT,
	CGI_PROSSESS,
	CGI_END
};

enum route_e {
	RT_NONE,
	RT_UPLOAD,
	RT_REDIR,
	RT_CGI
};

enum state_e {
	REQUEST_LINE,
	READING_HEADERS,
	READING_BODY,
	READY_TO_WRITE,
	WRITING, CLOSING, BAD
};

class State {
	public:
		State( short co , state_e st )
				{ code = co; state = st; }

		short			code;
		state_e			state;
};

#endif
