#include <iostream>
#include <vector>
#include "../includes/ConfigParser.hpp"
#include "../includes/ServerConfig.hpp"
#include "../includes/HttpResponseBuilder.hpp"
#include "../includes/HttpResponse.hpp"
#include "../includes/Client.hpp"

void printServers(const std::vector<ServerConfig>& servers);

int main(int argc, char* argv[]) {
    try {
	std::string config_file = "config/webserv.conf";

	if (argc > 1) {
	    config_file = argv[1];
	}

	ConfigParser parser(config_file);
	std::vector<ServerConfig> servers = parser.parseConfig();

/*********************************TESTS BLOCK********************************/
//NOTE: Building response
        HttpRequestData request;
        // Makeshift request
        request.method = "GET";
        request.full_path = "/";
        request.headers["head1"] = "head_one";


    //TODO: need a mechanism to determin which server to use
        HttpResponseBuilder builder(servers[0]);
    //NOTE: At the time let's consider that the line above is done
    //NOTE: my goal at the moment is to generate a response
        
        // Makeshift client
        Client test_client_001;
        std::vector<std::string> methods;
        methods.push_back("GET");
        methods.push_back("POST");

        test_client_001.location.setMethods(methods);
        test_client_001.location.setRoot("/var/www/html");
        test_client_001.location.setUploadLocation("/var/www/uploads");
        test_client_001.location.setAutoIndex(true);
        test_client_001.location.setUpload(true);
        test_client_001.location.setIndex("index.html");
        test_client_001.location.setReturn(301, "https://example.com");
        test_client_001.location.addCgi(".php", "/usr/bin/php-cgi");


        HttpResponse response = builder.buildResponse(request, test_client_001);

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

    } catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << std::endl;
	return 1;
    }
}


void printServers(const std::vector<ServerConfig>& servers) {
    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "=== Server " << i + 1 << " ===\n";

        // Print listen directives
        for (std::vector<std::pair<std::string,int> >::const_iterator it = servers[i].getListen().begin();
             it != servers[i].getListen().end(); ++it) {
            std::cout << "Listen: " << it->first << ":" << it->second << "\n";
        }

        // Print error pages
        for (std::map<int,std::string>::const_iterator it = servers[i].getErrorPages().begin();
             it != servers[i].getErrorPages().end(); ++it) {
            std::cout << "Error page " << it->first << " -> " << it->second << "\n";
        }

        // Print locations
        for (std::map<std::string,Location>::const_iterator it = servers[i].getLocations().begin();
             it != servers[i].getLocations().end(); ++it) {
            std::cout << "Location: " << it->first << "\n";
            std::cout << "  Root: " << it->second.getRoot() << "\n";
            std::cout << "  Index: " << it->second.getIndex() << "\n";
            std::cout << "  AutoIndex: " << (it->second.getAutoIndex() ? "on" : "off") << "\n";
            std::cout << "  Methods: ";
            for (size_t j = 0; j < it->second.getMethods().size(); ++j) {
                std::cout << it->second.getMethods()[j] << " ";
            }
            std::cout << "\n";
        }

        std::cout << "Max body size: " << servers[i].getMaxClientBodySize() << "\n\n";
    }
}

