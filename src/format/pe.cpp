#include <memory>
#include <iostream>

#include "pe.h"
#include "../util/util.h"

enum COFFCharacteristicFlags
{
	IMAGE_FILE_32BIT_MACHINE  = 0x0100

};


enum Machine
{
	IMAGE_FILE_MACHINE_UNKNOWN = 0x0,
	IMAGE_FILE_MACHINE_AMD64 = 0x8664,
	IMAGE_FILE_MACHINE_ARM = 0x1c0,
	IMAGE_FILE_MACHINE_ARM64 = 0xaa64,
	IMAGE_FILE_MACHINE_ARMNT = 0x1c4,
	IMAGE_FILE_MACHINE_I386 = 0x14c,
	IMAGE_FILE_MACHINE_IA64 = 0x200
};

enum SectionHeaderCharacteristicFlags
{
	IS_EXECUTABLE_CODE  = 0x0020

};

bool FormatPE::IsFormat(const std::vector<byte> * binDump)
{
	auto bd = *binDump;

	if(bd[0] == 0x4d
	&& bd[1] == 0x5a)
	{
		return true;
	}

	return false;
}

FormatPE::FormatPE(const std::vector<byte> * binDump)
{
	this->binDump = binDump;
	ParseBinDump();
}

void FormatPE::LoadMetadata(Metadata &metadata)
{
	metadata.format = FormatType::PE;
	metadata.end = EndType::LSB; // Assume all PE format files are LSB

	if (coffHeader.machine == IMAGE_FILE_MACHINE_I386)
	{
		metadata.arch = ArchType::x86;
	}
	else if (coffHeader.machine == IMAGE_FILE_MACHINE_AMD64)
	{
		metadata.arch = ArchType::x64;
	}
	else
	{
		metadata.arch = ArchType::UNDEFINED;
	}
}

Segment FormatPE::GetCodeSegment()
{
	Segment seg {};

	for (auto sh : sectionHeaders)
	{
		if (sh.characteristicFlags & IS_EXECUTABLE_CODE)
		{
			auto begin = binDump->begin() + sh.rawDataPointer;
			auto end = begin + sh.virtualSize; // Use virtualSize instead of rawSize because it doesn't have padding
			auto sectionCode = std::vector<byte>(begin, end);
			seg.seg->insert({sh.name, sectionCode});
		}
	}

	return seg;
}

void FormatPE::ParseBinDump()
{
	int offset = LoadCOFFHeader();
	LoadSectionHeaders(offset);
}

int FormatPE::LoadCOFFHeader()
{
	// Get the location of the offset to the COFF header, skipping the DOS stub/header
	// Add four to skip the second magic number, which I'm not sure why exists
	int offset = static_cast<int>(binDump->at(0x3C)) + 4;

	ByteSequence bs(binDump, offset);

	coffHeader.machine = bs.ReadBytes<uint16_t>();
	coffHeader.numSections = bs.ReadBytes<uint16_t>();
	coffHeader.timeDateStamp = bs.ReadBytes<uint32_t>();
	coffHeader.symbolTableOffset = bs.ReadBytes<uint32_t>();
	coffHeader.numSymbols = bs.ReadBytes<uint32_t>();
	coffHeader.optionalHeaderSize = bs.ReadBytes<uint16_t>();
	coffHeader.characteristicFlags = bs.ReadBytes<uint16_t>();

	if (coffHeader.optionalHeaderSize > 0)
	{
		// Skip the optional header
		return bs.offset + coffHeader.optionalHeaderSize;
	}

	else
	{
		std::cout << "ERROR: File provided is not a valid executable (potentially an object file?) Program will exit." << '\n';
		exit(EXIT_FAILURE);
	}
}

void FormatPE::LoadSectionHeaders(int offset)
{
	ByteSequence bs(binDump, offset);

	for (int i = 0; i < coffHeader.numSections; i++)
	{
		SectionHeader sh {};

		sh.name = bs.ReadString(8);
		sh.virtualSize = bs.ReadBytes<uint32_t>();
		sh.virtualAddress = bs.ReadBytes<uint32_t>();
		sh.rawSize = bs.ReadBytes<uint32_t>();
		sh.rawDataPointer = bs.ReadBytes<uint32_t>();
		sh.relocationPointer = bs.ReadBytes<uint32_t>();
		sh.lineNumberPointer = bs.ReadBytes<uint32_t>();
		sh.numRelocations = bs.ReadBytes<uint16_t>();
		sh.numLineNumbers = bs.ReadBytes<uint16_t>();
		sh.characteristicFlags = bs.ReadBytes<uint32_t>();

		sectionHeaders.push_back(sh);
	}
}

