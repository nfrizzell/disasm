#include <iostream>

#include "arch.h"
#include "x86/x86.h"

std::unique_ptr<Arch> Arch::NewArch(Segment seg, ArchType type)
{
	switch (type)
	{
		case (ArchType::x86):
			return std::make_unique<Arch_x86>(seg);
		default:
			std::cout << "Architecture not supported, program will exit...\n";
			exit(EXIT_FAILURE);
	}
}

