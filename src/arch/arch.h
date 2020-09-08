#pragma once

#include <map>
#include <vector>
#include <memory>

#include "../util/common.h"

class Arch
{
public:
	static std::unique_ptr<Arch> NewArch(const std::map<std::string, std::vector<byte>> * section, ArchType type);
	
	virtual std::vector<std::string> TranslateToAssembly() = 0;
	virtual std::vector<std::string> TranslateToSource() = 0;

protected:
	const std::map<std::string, std::vector<byte>> * segment;
};

