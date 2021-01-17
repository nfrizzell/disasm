#pragma once

#include <vector>
#include <map>
#include <memory>
#include <map>

#include "../util/common.h"

struct Segment
{
	std::map<std::string, std::vector<byte>> * seg;
};

class Format
{
public:
	static std::unique_ptr<Format> NewFormat(const std::vector<byte> * binDump, FormatType type);

	static FormatType GetFormatType(const std::vector<byte> * binPeek);

	virtual void LoadMetadata(Metadata &metadata) = 0;
	virtual Segment GetCodeSegment() = 0;

protected:
	const std::vector<byte> * binDump;

	virtual void ParseBinDump() = 0;
};

class FormatException : std::runtime_error 
{
	using std::runtime_error::exception;
};
