# include "Core.hpp"
# include "Server.hpp"
# include "ConfigParser.hpp"
# include "ServerConfig.hpp"

long getCurrentTimeMs() {
    struct timeval tv;

    if (gettimeofday(&tv, NULL) == -1) return 0;
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

time_t	Core::now_ms( void ) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (time_t)ts.tv_sec * 1000ull
		+ (time_t)ts.tv_nsec / 1000000ull;
}

int	main( int argc, char **argv ) {
	try {
		if (argc != 2) {
			std::cerr << "Usage: " << *argv << " <config_file>" << std::endl;
			return EXIT_FAILURE;
		}

		Server			server;
		ConfigParser	parser(argv[1]);

		server.create(parser.parseConfig());
		server.run();

		return EXIT_SUCCESS;
	}

	catch ( const std::exception &e ) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	catch ( ... ) {
		std::cerr << std::string(strerror(errno)) << std::endl;
	}

	return EXIT_FAILURE;
}
