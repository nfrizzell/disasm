#include <vector>

#include "common.h"

class ByteSequence
{
public:
	unsigned long offset;
	ByteSequence(const std::vector<byte> * binDump, unsigned long long offset, EndType endianness = EndType::LSB);

	template <typename T>
	T ReadBytes()
	{
		T ret = 0;
		if (endianness == EndType::LSB)
		{
			for (unsigned int i = 0; i < sizeof(T); i++)	
			{
				T a = ((*binDump)[offset + i] << (8 * i)); 	
				ret += a;
			}
		}

		else if (endianness == EndType::MSB)
		{
			// Implement when necessary
		}
		
		offset += sizeof(T);
		return ret;
	}

	std::string ReadString(int size);

private:
	EndType endianness;	
	const std::vector<byte> * binDump;

};
