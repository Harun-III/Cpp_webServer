# include "core.hpp"

# include "../includes/ConfigParser.hpp"
# include "../includes/ServerConfig.hpp"
# include "../includes/HttpResponseBuilder.hpp"
# include "../includes/HttpResponse.hpp"
# include "../includes/Client.hpp"

int	main( int argc, char **argv ) {
	try {
		if (argc != 2) {
			std::cerr << "Usage: " << *argv << " <config_file>" << std::endl;
			return EXIT_FAILURE;
		}

		std::string config_file(argv[1]);

		ConfigParser parser(config_file);
		std::vector<ServerConfig> servers = parser.parseConfig();

		// Server	server;
		//
		// server.create(servers);
		// server.run();

/*********************************TESTS BLOCK********************************/
//NOTE: Building response

	//TODO: need a mechanism to determine which server to use
		HttpResponseBuilder builder(servers[0]);
	//NOTE: At the time let's consider that the line above is done
	//NOTE: my goal at the moment is to generate a response
		
		// Makeshift client
		Client test_client_001;
		std::vector<std::string> methods;
		// methods.push_back("GET");
		methods.push_back("POST");

		test_client_001.location.setMethods(methods);
		test_client_001.location.setRoot("/var/www/html");
		test_client_001.location.setUploadLocation("/var/www/uploads");
		// test_client_001.location.setAutoIndex(false);
		test_client_001.location.setAutoIndex(true);
		test_client_001.location.setUpload(true);
		// test_client_001.location.setIndex("index.html");
		test_client_001.location.setReturn(301, "https://example.com");
		test_client_001.location.addCgi(".php", "/usr/bin/php-cgi");

		test_client_001.request.method = "GET";

		HttpResponse response = builder.buildResponse(test_client_001);

		/********TESTS BLOCK*********/
		// Makeshift response
		// HttpResponse response;
		// response.setStatusCode(400);
		//
		std::cout << response.toString() << std::endl;
		/****************************/
/****************************************************************************/


/*********************************TESTS BLOCK********************************/
/************************* print servers after parse ************************/
	// printServers(servers);
/****************************************************************************/

		return EXIT_SUCCESS;

	} catch ( const std::exception &e ) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch ( ... ) {
		std::cerr << "Error: all" << std::endl;
	}
}
