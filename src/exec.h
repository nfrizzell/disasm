#pragma once

#include <vector>
#include <memory>

#include "util/common.h"

#include "arch/arch.h"
#include "format/format.h"


class Executable
{
public:
	Executable(std::string path);
	~Executable();

private:
	Metadata metadata;
	std::vector<byte> * filePeek;
	std::vector<byte> * binDump;
	std::unique_ptr<Format> format;
	std::unique_ptr<Arch> arch;

	std::vector<byte> * PeekFile(std::string path);
	std::vector<byte> * LoadExecutable(std::string path);
};
