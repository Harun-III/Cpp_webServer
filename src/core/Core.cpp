# include "Core.hpp"
# include "Server.hpp"
# include "ConfigParser.hpp"
# include "ServerConfig.hpp"

int	main( int argc, char **argv ) {
	try {
		if (argc > 2) {
			std::cerr << "Usage: " << *argv << " [configuration file]" << std::endl;
			return EXIT_FAILURE;
		}

		std::string		configFile("./conf/basic.conf");
		if (argc == 2) configFile = argv[1];

		Server			server;
		ConfigParser	parser(configFile);

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
