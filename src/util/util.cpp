#include "util.h"

ByteSequence::ByteSequence(const std::vector<byte> * binDump, unsigned long long offset, EndType endianness)
{
	this->binDump = binDump;
	this->offset = offset;
	this->endianness = endianness;
}
