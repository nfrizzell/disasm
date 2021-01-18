#include <memory>
#include <iostream>

#include "pe.h"
#include "../util/util.h"

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
}

void FormatPE::LoadMetadata(Metadata &metadata)
{


}

Segment FormatPE::GetCodeSegment()
{


}

void FormatPE::ParseBinDump()
{


}

void FormatPE::LoadCOFFHeader()
{
	// Get the location of the offset to the COFF header, skipping the DOS stub/header
	// Add two to skip the second magic number, which I'm not sure why exists
	int offset = static_cast<int>(binDump->at(0x3C)) + 2;

	ByteSequence bs(binDump, offset);

	coffHeader.machine = bs.ReadBytes<uint16_t>();
	coffHeader.numSections = bs.ReadBytes<uint16_t>();
	coffHeader.timeDateStamp = bs.ReadBytes<uint32_t>();
	coffHeader.symbolTableOffset = bs.ReadBytes<uint32_t>();
	coffHeader.numSymbols = bs.ReadBytes<uint32_t>();
	coffHeader.optionalHeaderSize = bs.ReadBytes<uint16_t>();
	coffHeader.characteristicFlags = bs.ReadBytes<uint16_t>();

}

