# include "Core.hpp"

# include "ConfigParser.hpp"
# include "ServerConfig.hpp"
# include "HttpResponseBuilder.hpp"
# include "HttpResponse.hpp"
# include "Connection.hpp"

// NOTE: Delte these later
// #include "../core/core.hpp"
// #include "../../includes/ConfigParser.hpp"
// #include "../../includes/HttpResponseBuilder.hpp"
// #include "../../includes/HttpResponse.hpp"
// #include "../../includes/ServerConfig.hpp"
// #include "../../includes/Connection.hpp"

int	main( int argc, char **argv ) {
	try {
		if (argc != 2) {
			std::cerr << "Usage: " << *argv << " <config_file>" << std::endl;
			return EXIT_FAILURE;
		}

		ConfigParser parser(argv[1]);
		std::vector<ServerConfig> servers = parser.parseConfig();

		Server	server;

		server.create(servers);
		server.run();

		return EXIT_SUCCESS;

	} catch ( const std::exception &e ) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch ( ... ) {
		std::cerr << "Error: all" << std::endl;
	}
}
