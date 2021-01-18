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

class FormatPE : public Format
{
public:
	static bool IsFormat(const std::vector<byte> * binDump);

	FormatPE(const std::vector<byte> * binDump);
	void LoadMetadata(Metadata &metadata);
	Segment GetCodeSegment();
private:
	void ParseBinDump();
	void LoadCOFFHeader();

	COFFHeader coffHeader {};
};
