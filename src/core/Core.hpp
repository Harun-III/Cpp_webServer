#ifndef CORE_HPP
# define CORE_HPP

// ------------------------- System includes -------------------------//

# include <ctime>
# include <netdb.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <sys/epoll.h>

// ------------------------- Standard library -------------------------//

# include <map>
# include <vector>
# include <fstream>
# include <sstream>
# include <cstring>
# include <iostream>
# include <dirent.h>
# include <algorithm>

// ------------------------- Webserv Typedefs -------------------------//

typedef std::map<std::string, std::string>					map_s;
typedef struct addrinfo										addrinfo_t;
typedef std::vector< std::pair<std::string, std::string> >	vector_pairs;

// ------------------------- Webserv Macros -------------------------//

# define ERROR				-1
# define BUF_SIZE			(32 * 1024)
# define MAX_EVENTS			1024

# ifndef INACTIVITY_TIMEOUT_S
#  define INACTIVITY_TIMEOUT_S 10
# endif

# ifndef EPOLL_TIMEOUT_MS
#  define EPOLL_TIMEOUT_MS 1000
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

class Core {
	public:
		static volatile	sig_atomic_t g_stop;

		static void		onSignal( int ) {
			g_stop = true;
			write(STDERR_FILENO, "\r\033[K", 4);
		}
};

#endif
