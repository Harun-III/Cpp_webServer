#include <iostream>
#include <vector>
#include "../includes/ConfigParser.hpp"
#include "../includes/ServerConfig.hpp"

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
/************************* print servers after parse ************************/
	printServers(servers);
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

