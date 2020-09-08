#pragma once

#include <string>

using byte = unsigned char;
const std::string DATA_PATH = "../data/";
const int8_t INVALID = -127;

struct CLIFlags
{
	bool rawMachineCode {};
	bool debug {};
};

extern CLIFlags processFlags;

enum class FormatType
{
	UNDEFINED,
	ELF,
	PE
};

enum class EndType
{
	UNDEFINED,
	LSB,
	MSB
};

enum class ArchType
{
	UNDEFINED,
	x86,
	x64,
	ARM
};

struct Metadata 
{
	ArchType arch;	
	FormatType format;
	EndType end;
};
