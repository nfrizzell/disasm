#pragma once

#include <vector>

#include "../util/common.h"
#include "format.h"

// Reference:
// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format

struct COFFHeader
{
	uint16_t machine;
	uint16_t numSections;
	uint32_t timeDateStamp;
	uint32_t symbolTableOffset;
	uint32_t numSymbols;
	uint16_t optionalHeaderSize;
	uint16_t characteristicFlags;
};

struct SectionHeader
{
	std::string name;
	uint32_t virtualSize;
	uint32_t virtualAddress;
	uint32_t rawSize;
	uint32_t rawDataPointer;
	uint32_t relocationPointer; // Unused
	uint32_t lineNumberPointer; // Unused
	uint16_t numRelocations; // Unused
	uint16_t numLineNumbers; // Unused
	uint32_t characteristicFlags;
};

class FormatPE : public Format
{
public:
	static bool IsFormat(const std::vector<byte> * binDump);

	FormatPE(const std::vector<byte> * binDump);
	void LoadMetadata(Metadata &metadata);
	Segment GetCodeSegment();
private:
	COFFHeader coffHeader {};
	std::vector<SectionHeader> sectionHeaders {};

	void ParseBinDump();
	int LoadCOFFHeader();
	void LoadSectionHeaders(int offset);
};
