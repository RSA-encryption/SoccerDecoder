#include <iostream>
#include "headers/SoccerDecoder.hpp"

#define BUFSIZE 4096

int main(int argc, char* argv[])
{
	try {
		SoccerDecoder soccer(argc, argv);
		soccer.Start();
		printf("%s", soccer.GetResultJson().c_str()); // Dump to stdout
	} catch (const std::invalid_argument& e) {
		(void)e; // Mark as used to avoid C4101 warning
		std::cerr << "Error : invalid_argument must be a integer" << std::endl;
		exit(EXIT_FAILURE);
	} catch (const std::out_of_range& e) {
		(void)e;
		std::cerr << "Error : out of range" << std::endl;
		exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
