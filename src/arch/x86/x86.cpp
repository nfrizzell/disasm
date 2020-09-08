#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>

#include "x86.h"
#include "csv.h"

using namespace ISet_x86;

// ***** Arch_x86 *****
Arch_x86::Arch_x86(const std::map<std::string, std::vector<byte>> * segment)
{
	x86CSVParse(instrReference);

	this->segment = segment;
}

std::vector<std::string> Arch_x86::TranslateToAssembly()
{
	assembly = std::vector<std::string>();
	for (auto section : (*segment))
	{
		if (section.first == ".init")
		{
			auto decoder = LinearDecoder(&section.second);
			instructions = decoder.DecodeSection();
			//auto translator = Translator(instructions);
			//assembly = translator.TranslateToASM();

			if (processFlags.debug)
			{
				// Write decoded data
			}
		}
	}

	return assembly;
}

std::vector<std::string> Arch_x86::TranslateToSource()
{
	throw std::runtime_error("Method 'TranslateToSource' unimplemented");
}
	
std::vector<Instruction> Arch_x86::GetInstructionData()
{
	if (instructions.size() == 0)
	{
		throw std::logic_error("Attempt to call method 'GetInstructionData' before initializing instructions");
	}

	return instructions;
}
