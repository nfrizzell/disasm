#include "translate.h"
#include <sstream>
#include <algorithm>
#include <string>
#include <iomanip>

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

Translator::Translator(std::vector<Instruction> decodedInstrs, std::vector<byte> * section)
{
	this->decodedInstrs = decodedInstrs;
	this->section = section;
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
	std::stringstream line {};

	line << std::setw(32) << std::left;
	std::stringstream bytes {};
	for (int i = instr.attrib.runtime.segmentByteOffset; i < instr.attrib.runtime.segmentByteOffset + instr.attrib.runtime.size; i++)
	{
		bytes << std::hex << (int)section->at(i) << " ";
	}

	line << bytes.str() << instr.attrib.intrinsic.mnemonic;

    if (instr.op1.attrib.intrinsic.type != OperandType::NOT_APPLICABLE)
    {
		std::string opStr = StringifyOperand(instr, instr.op1);
		line << '\t' << opStr;
    }

    if (instr.op2.attrib.intrinsic.type != OperandType::NOT_APPLICABLE)
    {
		std::string opStr = StringifyOperand(instr, instr.op2);
		line << "," << opStr;
    }

    if (instr.op3.attrib.intrinsic.type != OperandType::NOT_APPLICABLE)
    {
		std::string opStr = StringifyOperand(instr, instr.op3);
		line << "," << opStr;
    }

    if (instr.op4.attrib.intrinsic.type != OperandType::NOT_APPLICABLE)
    {
		std::string opStr = StringifyOperand(instr, instr.op4);
		line << "," << opStr;
    }

	std::cout << line.str() << "\n";
	return line.str();
}

std::string Translator::StringifyOperand(const Instruction &instr, const Operand &op)
{
	if (op.attrib.runtime.encoding == Operand::IMMD)
	{
		std::stringstream immdStrStrm;
		immdStrStrm << "0x" << std::hex << instr.encoded.immd;
		return immdStrStrm.str();
	}

	if (op.attrib.runtime.encoding == Operand::RELATIVE_DISPLACEMENT)
	{
		std::stringstream addrStrStrm;
		int relativeDisplacement = instr.encoded.immd;

		addrStrStrm << "<0x" << std::hex << relativeDisplacement << " + relative address>";
		return addrStrStrm.str();
	}

	else if (op.attrib.runtime.encoding == Operand::OPCODE_REGISTER)
	{
		AddrMethod encodedReg = ModRMRegisterEncoding32.at(instr.encoded.opcode.primary & 0b00000111);
		return regString.at(encodedReg);
	}

	else if (op.attrib.runtime.encoding == Operand::MODRM_REGISTER_REGBITS)
	{
		AddrMethod reg = ModRMRegisterEncoding32.at(instr.encoded.modrm.regOpBits);
		return regString.at(reg);
	}

	else if (op.attrib.runtime.encoding == Operand::MODRM_REGISTER_RMBITS)
	{
		AddrMethod reg = ModRMRegisterEncoding32.at(instr.encoded.modrm.rmBits);
		return regString.at(reg);
	}

	else if (op.attrib.runtime.encoding == Operand::MODRM_REGISTER_WITH_DISP)
	{

	}

	else if (op.attrib.runtime.encoding == Operand::MODRM_REGISTER_SCALED)
	{

	}

	else if (op.attrib.runtime.encoding == Operand::MODRM_REGISTER_SCALED_WITH_DISP)
	{

	}
}

};
