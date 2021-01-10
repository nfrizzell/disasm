#include "translate.h"
#include <sstream>
#include <algorithm>
#include <string>

namespace ISet_x86
{

const std::map<AddrMethod, std::string> regString
{
	{ AddrMethod::A, "ERROR" },
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
	auto line = std::stringstream();
	
	std::string mnemonic = instr.attrib.intrinsic.mnemonic;
	std::transform(mnemonic.begin(), mnemonic.end(), mnemonic.begin(), ::tolower);
	line << mnemonic;

	line << '\t' << StringifyOperand(instr, instr.op1, true);
	line << StringifyOperand(instr, instr.op2, false);
	line << StringifyOperand(instr, instr.op3, false);
	line << StringifyOperand(instr, instr.op4, false);

	std::cout << line.str() << '\n';
	return line.str();
}

std::string Translator::StringifyOperand(const Instruction &instr, const Operand &op, const bool isOp1)
{
	auto opStr = std::stringstream();
	opStr << "";

	if (op.attrib.intrinsic.type == OperandType::NOT_APPLICABLE)
	{
		return "";
	}

	else if (op.attrib.runtime.regValue != AddrMethod::NOT_APPLICABLE)
	{
		std::string next = regString.at(op.attrib.runtime.regValue);
		opStr << next;
	}

	else if (op.attrib.runtime.isImmd)
	{
		opStr << "0x" << std::hex << instr.encoded.immd;
	}

	if (op.attrib.runtime.isAddress)
	{
		std::string bracketStr = "[" + opStr.str() + "]";
		opStr.str("");
		opStr << bracketStr;
	}

	if (!isOp1 && opStr.str() != "")
	{
		std::string commaStr = "," + opStr.str();
		opStr.str("");
		opStr << commaStr;
	}

	return opStr.str();
}

};
