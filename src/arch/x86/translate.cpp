#include "translate.h"

namespace ISet_x86
{

const std::map<AddrMethod, std::string> regString
{
	{ AddrMethod::AH, "ah" },
	{ AddrMethod::AL, "al" },
	{ AddrMethod::AX, "ax" },
	{ AddrMethod::EAX, "eax" },
	{ AddrMethod::BH, "bh" },
	{ AddrMethod::BL, "bl" },
	{ AddrMethod::BX, "bx" },
	{ AddrMethod::EBX, "ebx" },
	{ AddrMethod::CH, "ch" },
	{ AddrMethod::CL, "cl" },
	{ AddrMethod::CX, "cx" },
	{ AddrMethod::ECX, "ecx" },
	{ AddrMethod::DH, "dh" },
	{ AddrMethod::DL, "dl" },
	{ AddrMethod::DX, "dx" },
	{ AddrMethod::EDX, "edx" },
	{ AddrMethod::SI, "si" },
	{ AddrMethod::ESI, "esi" },
	{ AddrMethod::DI, "di" },
	{ AddrMethod::EDI, "edi" },
	{ AddrMethod::BP, "bp" },
	{ AddrMethod::EBP, "ebp" },
	{ AddrMethod::SP, "sp" },
	{ AddrMethod::ESP, "esp" },
	{ AddrMethod::SS, "ss" },
	{ AddrMethod::CS, "cs" },
	{ AddrMethod::DS, "ds" },
	{ AddrMethod::ES, "fs" },
	{ AddrMethod::GS, "gs" },
	{ AddrMethod::IP, "ip" },
	{ AddrMethod::EIP, "eip" },
	{ AddrMethod::EFLAGS, "eflags" }
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
		opStr = regString.at(op.attrib.runtime.regValue);
	}
	
	if (op.attrib.runtime.isAddress)
	{
		opStr = ("[" + opStr + "]");
	}

	line += (" " + opStr);
}

};
