#include "arch.h"
#include "x86/x86.h"

std::unique_ptr<Arch> Arch::NewArch(const std::map<std::string, std::vector<byte>> * section, ArchType type)
{
	switch (type)
	{
		case (ArchType::x86):
			return std::make_unique<Arch_x86>(section);
		default:
			return std::make_unique<Arch_x86>(section);

	}
	
}

