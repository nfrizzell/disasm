#include <array>
#include <stdexcept>

#include "format.h"
#include "elf.h"

FormatType Format::GetFormatType(const std::vector<byte> * binPeek)
{
	if (FormatELF::IsFormat(binPeek))
	{
		return FormatType::ELF;		
	}		
			
	throw std::runtime_error("File format not implemented");
}

std::unique_ptr<Format> Format::NewFormat(const std::vector<byte> * binDump, FormatType type)
{
	switch (type)
	{
		case (FormatType::ELF):
			return FormatELF::NewFormatELF(binDump);		
		default:
			throw std::runtime_error("File format unrecognized");
	}
}
