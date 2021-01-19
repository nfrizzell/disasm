#include "util.h"

#include <iostream>

ByteSequence::ByteSequence(const std::vector<byte> * binDump, unsigned long long offset, EndType endianness)
{
	this->binDump = binDump;
	this->offset = offset;
	this->endianness = endianness;
}

std::string ByteSequence::ReadString(int size)
{
	std::string out {};
	for (int i = 0; i < size; i++)
	{
		if ((*binDump)[offset + i] != 0)
		{
			out += (*binDump)[offset + i];
		}
	}

	offset += size;

	return out;
}
