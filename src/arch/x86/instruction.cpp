#include <stdexcept>
#include <iostream>
#include <cmath>
#include <sstream>

#include "instruction.h"

namespace ISet_x86
{

// Used to figure out what entry the encoded instruction maps to
bool Opcode::operator==(const Opcode &rhs) const
{
	return (mandatoryPrefix == rhs.mandatoryPrefix &&
		twoByte == rhs.twoByte && 
		primary == rhs.primary && 
		secondary == rhs.secondary &&
		extension == rhs.extension);
}

std::size_t OpcodeHash::operator()(const Opcode &op) const
{
	auto ss = std::stringstream();

	ss << op.mandatoryPrefix << (int)op.twoByte << op.primary << op.secondary << op.extension;

	return std::hash<std::string>()(ss.str());
}

bool ThreeByteKey::operator==(const ThreeByteKey &tbk) const
{
	return twoByte == tbk.twoByte && primary == tbk.primary;
}

std::size_t ThreeByteHash::operator()(const ThreeByteKey &tbk) const
{
	auto ss = std::stringstream();

	ss << tbk.twoByte << tbk.primary;

	return std::hash<std::string>()(ss.str());
}

// After retrieving the reference attribute from the reference set, update the current instruction that
// is being constructed's attributes
void Instruction::UpdateAttributes(const Instruction &reference)
{
	attrib.intrinsic = reference.attrib.intrinsic;
	op1 = reference.op1;
	op2 = reference.op2;
	op3 = reference.op3;
	op4 = reference.op4;
	encoded.opcode.extension = reference.encoded.opcode.extension;
}

void Instruction::InterpretModRMByte(const byte modrmByte)
{
	attrib.flags.modRMRead = true;

	// The field in the ModR/M byte to read from to get the relevant encoded operand
	// If either operand is of type immediate, change the ModR/M field to be read from to
	// R/M, since REG is used by the opcode instead
	// ...
	// If the ModRM byte has already been read (op1 and op2 both need to read it), also
	// do this, since the operand to be checked will be encoded in a different field
	// Bit-shifted for easier comparisons
	// Right bit shift on an unsigned integer results in a logical right shift

	encoded.modrm.modBits   = (modrmByte & 0b11000000) >> 6; 
	encoded.modrm.regOpBits = (modrmByte & 0b00111000) >> 3;
	encoded.modrm.rmBits    = (modrmByte & 0b00000111);

	if (encoded.modrm.modBits == 0b00)
	{
		if (encoded.modrm.rmBits == 0b101)
		{
			attrib.flags.hasDisplacement = true;
			attrib.runtime.displacementSize = 4;
		}
	}

	// Operand is located at a memory address found by combining a specified register and a one-byte displacement
	else if (encoded.modrm.modBits == 0b01)
	{
		attrib.flags.hasDisplacement = true;
		attrib.runtime.displacementSize = 1;
	}
	
	// Operand is located at a memory address found by combining a specified register and a four-byte displacement
	else if (encoded.modrm.modBits == 0b10)
	{
		attrib.flags.hasDisplacement = true;
		attrib.runtime.displacementSize = 4;
	}

	// SIB follows
	if (encoded.modrm.modBits != 0b11 && encoded.modrm.rmBits == 0b100)
	{
		attrib.flags.hasSIB = true;
	}

    // Retrieve the actual opext if it exists, replacing the placeholder
    // Afterwards, update the relevant attributes (such as mnemonic) using
    // the new information
	if (encoded.opcode.extension != INVALID)
	{

		encoded.opcode.extension = encoded.modrm.regOpBits;
		auto reference = instrReference.GetReference(encoded.opcode);
		UpdateAttributes(reference);
	}

}

void Instruction::InterpretSIBByte(byte sibByte)
{
	attrib.flags.sibRead = true;

	// Bit-shifted for easier comparisons
	// Right bit shift on an unsigned integer results in a logical right shift
	encoded.sib.scaleBits = (sibByte & 0b11000000) >> 6;
	encoded.sib.indexBits = (sibByte & 0b00111000) >> 3;
	encoded.sib.baseBits  = (sibByte & 0b00000111);

	// Base
	if (encoded.sib.baseBits == 0b101 && encoded.modrm.modBits == 0b00)
	// Displacement-only
	{
			
	}

	else if (encoded.sib.baseBits == 0b101 && (encoded.modrm.modBits == 0b10 && encoded.modrm.modBits == 0b01))
	{
		//op2.attrib.runtime.regValue = AddrMethod::EBP;
	}

	else
	{
		//op2.attrib.runtime.regValue = SIBBase.at(encoded.sib.baseBits);
	}
}

std::ostream & operator<<(std::ostream &out, const Instruction &instr)
{
	out << std::hex << "mnem:" << '\t' << instr.attrib.intrinsic.mnemonic << '\n' 
	<< "prefix:" << '\t' << instr.encoded.opcode.mandatoryPrefix << '\n' 
	<< "2byte:" << '\t' << (int)instr.encoded.opcode.twoByte << '\n' 
	<< "popcd:" << '\t' << instr.encoded.opcode.primary << '\n' 
	<< "sopcd:" << '\t' << instr.encoded.opcode.secondary << '\n' 
	<< "opext:" << '\t' << (int)instr.encoded.opcode.extension << '\n';

    out << std::dec << '\t' << "op1: " << (int)instr.op1.attrib.intrinsic.addrMethod << " " << (int)instr.op1.attrib.intrinsic.type << '\n';
    out << std::dec << '\t' << "op2: " << (int)instr.op2.attrib.intrinsic.addrMethod << " " << (int)instr.op2.attrib.intrinsic.type << '\n';
    out << std::dec << '\t' << "op3: " << (int)instr.op3.attrib.intrinsic.addrMethod << " " << (int)instr.op3.attrib.intrinsic.type << '\n';
    out << std::dec << '\t' << "op4: " << (int)instr.op4.attrib.intrinsic.addrMethod << " " << (int)instr.op4.attrib.intrinsic.type << '\n';

	out << std::hex << "displacement: " << instr.encoded.disp << '\n';
	out << std::hex << "immd: " << instr.encoded.immd << '\n';

	return out;
}

std::ostream & operator<<(std::ostream &out, const Opcode &opcode)
{
	out << std::hex << "prefix:" << '\t' << opcode.mandatoryPrefix << '\n' 
	<< "2byte:" << '\t' << (int)opcode.twoByte << '\n' 
	<< "popcd:" << '\t' << opcode.primary << '\n' 
	<< "sopcd:" << '\t' << opcode.secondary << '\n' 
	<< "opext:" << '\t' << (int)opcode.extension << '\n';

	return out;
}

};


