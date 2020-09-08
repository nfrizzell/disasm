#include "translate.h"

namespace ISet_x86
{

const std::map<OperandType, std::string> regString
{
	{ OperandType::AH, "ah" },
	{ OperandType::AL, "al" },
	{ OperandType::AX, "ax" },
	{ OperandType::EAX, "eax" },
	{ OperandType::BH, "bh" },
	{ OperandType::BL, "bl" },
	{ OperandType::BX, "bx" },
	{ OperandType::EBX, "ebx" },
	{ OperandType::CH, "ch" },
	{ OperandType::CL, "cl" },
	{ OperandType::CX, "cx" },
	{ OperandType::ECX, "ecx" },
	{ OperandType::DH, "dh" },
	{ OperandType::DL, "dl" },
	{ OperandType::DX, "dx" },
	{ OperandType::EDX, "edx" },
	{ OperandType::SI, "si" },
	{ OperandType::ESI, "esi" },
	{ OperandType::DI, "di" },
	{ OperandType::EDI, "edi" },
	{ OperandType::BP, "bp" },
	{ OperandType::EBP, "ebp" },
	{ OperandType::SP, "sp" },
	{ OperandType::ESP, "esp" },
	{ OperandType::SS, "ss" },
	{ OperandType::CS, "cs" },
	{ OperandType::DS, "ds" },
	{ OperandType::ES, "fs" },
	{ OperandType::GS, "gs" },
	{ OperandType::IP, "ip" },
	{ OperandType::EIP, "eip" },
	{ OperandType::EFLAGS, "eflags" }
};

Translator::Translator(std::vector<Instruction> decodedInstrs)
{
	this->decodedInstrs = decodedInstrs;

}

std::vector<std::string> Translator::TranslateToASM()
{
	std::vector<std::string> translatedAsm;

	for (auto instruction : decodedInstrs)
	{
		std::string line = StringifyInstruction(instruction);
		translatedAsm.push_back(line);
	}

	return translatedAsm;
}

std::string Translator::StringifyInstruction(const Instruction &instr)
{
	std::string line = instr.attrib.intrinsic.mnemonic;

	StringifyOperand(line, instr, instr.op1, true);
	StringifyOperand(line, instr, instr.op2, false);

	return line;
}

void Translator::StringifyOperand(std::string &line, const Instruction &instr, const Operand &op, const bool isOp1)
{
	std::string opStr {};

	if (op.attrib.intrinsic.type == OperandType::NOT_APPLICABLE)
	{
		return;
	}

	if (op.attrib.runtime.isRegister)
	{
		opStr = regString.at(static_cast<OperandType>(op.value));
	}
	
	if (op.attrib.runtime.isAddress)
	{
		opStr = ("[" + opStr + "]");
	}

	line += (" " + opStr);
}

};
