#pragma once

#include <map>
#include <vector>
#include <memory>

#include "../util/common.h"
#include "../format/format.h"

class Arch
{
public:
	static std::unique_ptr<Arch> NewArch(Segment seg, ArchType type);
	
	virtual std::vector<std::string> TranslateToAssembly() = 0;
	virtual std::vector<std::string> TranslateToSource() = 0;

protected:
	Segment segment;
};

