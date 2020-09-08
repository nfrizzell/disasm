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
	Translator(std::vector<Instruction> decodedInstrs);

	std::vector<std::string> TranslateToASM();

private:
	std::vector<Instruction> decodedInstrs;

	std::string StringifyInstruction(const Instruction &instr);
	void StringifyOperand(std::string &line, const Instruction &instr, const Operand &op, const bool isOp1);
};

};
