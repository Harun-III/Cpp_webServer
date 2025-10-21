# include "Core.hpp"
# include "Server.hpp"
# include "ConfigParser.hpp"
# include "ServerConfig.hpp"

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
