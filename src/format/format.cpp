#include <array>
#include <stdexcept>
#include <iostream>

#include "format.h"
#include "elf.h"
#include "pe.h"

FormatType Format::GetFormatType(const std::vector<byte> * binPeek)
{
	if (FormatELF::IsFormat(binPeek))
	{
		return FormatType::ELF;		
	}		

	else if (FormatPE::IsFormat(binPeek))
	{
		return FormatType::PE;
	}
			
	throw std::runtime_error("File format not implemented");
}

std::unique_ptr<Format> Format::NewFormat(const std::vector<byte> * binDump, FormatType type)
{
	switch (type)
	{
		case (FormatType::ELF):
			return FormatELF::NewFormatELF(binDump);
			break;
		case (FormatType::PE):
			return std::make_unique<FormatPE>(binDump);
			break;
		default:
			std::cout << "File format not supported, program will exit...";
			exit(EXIT_FAILURE);
	}
}
