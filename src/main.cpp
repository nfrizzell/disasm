#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

#include "util/common.h"
#include "exec.h"

class ProcessInstance 
{
public:
	ProcessInstance(std::string path)
	{
		this->exec = new Executable(path);
	}

private:
	Executable * exec;
};

void ParseFlags(int argc, const char * argv[])
{
	const std::vector<std::string> validFlags = {"-d"};

	// No path or other arguments supplied
	if (argc == 1)
	{
		std::cout << "ERROR: Incorrect number of arguments. Did you supply the executable path?" << '\n';
		exit(EXIT_FAILURE);
	}

	// Path and other arguments supplied
	else if (argc > 2)
	{
		// Iterate over valid flag indices
		for (int i = 1; i < argc - 1; i++)
		{
			std::string arg(argv[i]);
			// If invalid argument
			if (std::find(validFlags.begin(), validFlags.end(), arg) == validFlags.end())	
			{
				std::cout << "ERROR: Invalid arguments supplied." << '\n';
				exit(EXIT_FAILURE);
			}

			// Debug mode (to be implemented)
			else if (arg == "-d")
			{
				processFlags.debug = true;
			}
		}
	}

}

int main(int argc, const char * argv[])
{
	ParseFlags(argc, argv);

	auto process = ProcessInstance(std::string(argv[argc-1]));
}	
