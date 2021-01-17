#pragma once

#include <vector>

#include "decode.h"
#include "instruction.h"

namespace ISet_x86
{

class Translator
// Intel syntax
{
public:
	Translator(std::vector<Instruction> decodedInstrs, std::vector<byte> * section);

	std::vector<std::string> TranslateToASM();

private:
	std::vector<Instruction> decodedInstrs;
	std::vector<byte> * section;

	std::string StringifyInstruction(const Instruction &instr);
	std::string StringifyOperand(const Instruction &instr, const Operand &op);
};

};
