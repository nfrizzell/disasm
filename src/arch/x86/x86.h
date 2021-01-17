#pragma once

#include <memory>
#include <map>

#include "../../util/common.h"
#include "../arch.h"
#include "instruction.h"
#include "decode.h"
#include "translate.h"

class Arch_x86 final : public Arch
{
public:
	Arch_x86(Segment segment);

	std::vector<std::string> TranslateToAssembly();
	std::vector<std::string> TranslateToSource();

	std::vector<ISet_x86::Instruction> GetInstructionData();

private:
	std::vector<ISet_x86::Instruction> instructions;
	std::vector<std::string> assembly;

	friend class ISet_x86::LinearDecoder;
};

