#include <iostream>
#include "../includes/ConfigParser.hpp"

int main(int argc, char* argv[]) {
    try {
	std::string config_file = "../config/webserv.conf";

	if (argc > 1) {
	    config_file = argv[1];
	}

// Here we should call the parser
	ConfigParser parser(config_file);
	parser.parseConfig();


    } catch (const std::exception& e) {
	std::cerr << "Error: " << e.what() << std::endl;
	return 1;
    }
}

