#include <stdexcept>
#include <iostream>

#include "instruction.h"

namespace ISet_x86
{

std::map<OperandType, uint8_t> typeSize16
{
	{OperandType::a, 4},
	{OperandType::b, 1},
	{OperandType::c, 1},
	{OperandType::d, 4},
	{OperandType::dq, 16},
	{OperandType::p, 4},
	{OperandType::pi, 8},
	{OperandType::ps, 16},
	{OperandType::q, 8},
	{OperandType::s, 6},
	{OperandType::ss, 16},
	{OperandType::si, 4},
	{OperandType::v, 2},
	{OperandType::w, 2},
	{OperandType::x, 8}, 
	{OperandType::y, 4}, 
	{OperandType::z, 2},	
	{OperandType::AH, 2},
	{OperandType::AL, 2},
	{OperandType::AX, 2},
	{OperandType::EAX, 4},
	{OperandType::BH, 2},
	{OperandType::BL, 2},
	{OperandType::BX, 2},
	{OperandType::EBX, 4},
	{OperandType::CH, 2},
	{OperandType::CL, 2},
	{OperandType::CX, 2},
	{OperandType::ECX, 4},
	{OperandType::DH, 2},
	{OperandType::DL, 2},
	{OperandType::DX, 2},
	{OperandType::EDX, 4},
	{OperandType::SI, 2},
	{OperandType::ESI, 4},
	{OperandType::DI, 2},
	{OperandType::EDI, 4},
	{OperandType::BP, 2},
	{OperandType::EBP, 4},
	{OperandType::SP, 2},
	{OperandType::ESP, 4},
	{OperandType::SS, 2},
	{OperandType::CS, 2},
	{OperandType::DS, 2},
	{OperandType::ES, 2},
	{OperandType::FS, 2},
	{OperandType::GS, 2},
	{OperandType::IP, 2},
	{OperandType::EIP, 4},
	{OperandType::EFLAGS, 2}
};

std::map<OperandType, uint8_t> typeSize32
{
	{OperandType::a, 8},
	{OperandType::b, 1},
	{OperandType::c, 2},
	{OperandType::d, 4},
	{OperandType::dq, 16},
	{OperandType::p, 6},
	{OperandType::pi, 8},
	{OperandType::ps, 16},
	{OperandType::q, 8},
	{OperandType::s, 6},
	{OperandType::si, 4},
	{OperandType::ss, 16},
	{OperandType::v, 4},
	{OperandType::w, 2},
	{OperandType::x, 16}, 
	{OperandType::y, 8}, 
	{OperandType::z, 8},	
	{OperandType::AH, 2},
	{OperandType::AL, 2},
	{OperandType::AX, 2},
	{OperandType::EAX, 4},
	{OperandType::BH, 2},
	{OperandType::BL, 2},
	{OperandType::BX, 2},
	{OperandType::EBX, 4},
	{OperandType::CH, 2},
	{OperandType::CL, 2},
	{OperandType::CX, 2},
	{OperandType::ECX, 4},
	{OperandType::DH, 2},
	{OperandType::DL, 2},
	{OperandType::DX, 2},
	{OperandType::EDX, 4},
	{OperandType::SI, 2},
	{OperandType::ESI, 4},
	{OperandType::DI, 2},
	{OperandType::EDI, 4},
	{OperandType::BP, 2},
	{OperandType::EBP, 4},
	{OperandType::SP, 2},
	{OperandType::ESP, 4},
	{OperandType::SS, 2},
	{OperandType::CS, 2},
	{OperandType::DS, 2},
	{OperandType::ES, 2},
	{OperandType::FS, 2},
	{OperandType::GS, 2},
	{OperandType::IP, 2},
	{OperandType::EIP, 4},
	{OperandType::EFLAGS, 2}
};

// Defined values of the REG field of the ModR/M byte if the data is 8 bits
const std::map<int, OperandType> ModRMRegisterEncoding8
{
	{ 0b000, OperandType::AL },
	{ 0b001, OperandType::CL },
	{ 0b010, OperandType::DL },
	{ 0b011, OperandType::BL },
	{ 0b100, OperandType::AH },
	{ 0b101, OperandType::CH },
	{ 0b110, OperandType::DH },
	{ 0b111, OperandType::BH }
};

// Defined values of the REG field of the ModR/M byte if the data is 16 bits
const std::map<int, OperandType> ModRMRegisterEncoding16
{
	{ 0b000, OperandType::AX },
	{ 0b001, OperandType::CX },
	{ 0b010, OperandType::DX },
	{ 0b011, OperandType::BX },
	{ 0b100, OperandType::SP },
	{ 0b101, OperandType::BP },
	{ 0b110, OperandType::SI },
	{ 0b111, OperandType::DI }
};

// Defined values of the REG field of the ModR/M byte if the data is 32 bits
const std::map<int, OperandType> ModRMRegisterEncoding32
{
	{ 0b000, OperandType::EAX },
	{ 0b001, OperandType::ECX },
	{ 0b010, OperandType::EDX },
	{ 0b011, OperandType::EBX },
	{ 0b100, OperandType::ESP },
	{ 0b101, OperandType::EBP },
	{ 0b110, OperandType::ESI },
	{ 0b111, OperandType::EDI }
};

const std::map<int, int> SIBScaleFactor
{
	{0b00, 1},
	{0b01, 2},
	{0b10, 4},
	{0b11, 8}
};

const std::map<int, OperandType> SIBIndex = ModRMRegisterEncoding32;
const std::map<int, OperandType> SIBBase = ModRMRegisterEncoding32;

bool Opcode::operator==(const Opcode &rhs) const
{
	return (mandatoryPrefix == rhs.mandatoryPrefix &&
		twoByte == rhs.twoByte && 
		primary == rhs.primary && 
		secondary == rhs.secondary &&
		weight == rhs.weight);
}

bool Opcode::operator<(const Opcode &rhs) const
{
	bool ret = false;
	ret = (mandatoryPrefix < rhs.mandatoryPrefix) ? true : ret;
	ret = (primary < rhs.primary) ? true : ret;
	ret = (!twoByte && rhs.twoByte) ? true : ret;
	ret = (secondary < rhs.secondary) ? true : ret;
	ret = (weight < rhs.weight) ? true : ret;

	return ret;
}

void Operand::UpdateSize(std::array<byte, 4> &prefixes)
{
	if (attrib.intrinsic.type == OperandType::NOT_APPLICABLE)
	{
		attrib.intrinsic.size = 0;
		return;
	}

	bool addrSizePrefix {};
	bool operandSizePrefix {}; 

	for (auto prefix : prefixes)
	{
		if (prefix == 0x66)
		{
			addrSizePrefix = true;
		}
		
		else if (prefix == 0x67)
		{
			operandSizePrefix = true;
		}
	}

	bool is32 = addrSizePrefix || operandSizePrefix;
	attrib.intrinsic.size = is32 ? typeSize32.at(attrib.intrinsic.type) : typeSize16.at(attrib.intrinsic.type);
}

void Instruction::UpdateAttributes(const Instruction &reference)
{
	attrib.intrinsic = reference.attrib.intrinsic;
	attrib.runtime.opcodeLength = reference.attrib.runtime.opcodeLength;

	op1.attrib.intrinsic = reference.op1.attrib.intrinsic;
	op1.attrib.runtime.isRegister = reference.op1.attrib.runtime.isRegister;
	op1.UpdateSize(encoded.prefix);

	op2.attrib.intrinsic = reference.op2.attrib.intrinsic;
	op2.attrib.runtime.isRegister = reference.op2.attrib.runtime.isRegister;
	op2.UpdateSize(encoded.prefix);

	op3.attrib.intrinsic = reference.op3.attrib.intrinsic;
	op3.attrib.runtime.isRegister = reference.op3.attrib.runtime.isRegister;
	op3.UpdateSize(encoded.prefix);

	op4.attrib.intrinsic = reference.op4.attrib.intrinsic;
	op4.attrib.runtime.isRegister = reference.op4.attrib.runtime.isRegister;
	op4.UpdateSize(encoded.prefix);
	
}

void Instruction::InterpretModRMByte(const byte modrmByte)
{
	// The field in the ModR/M byte to read from to get the relevant encoded operand
	uint8_t * operandField = &encoded.modrm.regOpBits;

	// If either operand is of type immediate, change the ModR/M field to be read from to
	// R/M, since REG is used by the opcode instead
	// ...
	// If the ModRM byte has already been read (op1 and op2 both need to read it), also
	// do this, since the operand to be checked will be encoded in a different field
	if ((op1.attrib.intrinsic.addrMethod == AddrMethod::I || op2.attrib.intrinsic.addrMethod == AddrMethod::I) || attrib.runtime.modRMRead)
	{
		operandField = &encoded.modrm.rmBits;
	}

	// Bit-shifted for easier comparisons
	// Right bit shift on an unsigned integer results in a logical right shift
	encoded.modrm.modBits   = (modrmByte & 0b11000000) >> 6; 
	encoded.modrm.regOpBits = (modrmByte & 0b00111000) >> 3;
	encoded.modrm.rmBits    = (modrmByte & 0b00000111);

	// Check for SIB mode
	attrib.runtime.hasSIB = (encoded.modrm.rmBits == 0b100 && encoded.modrm.modBits != 0b11);

	if (encoded.modrm.modBits == 0b00)
	{
		if (encoded.modrm.rmBits == 0b000 || encoded.modrm.rmBits == 0b111)
		{
			activeOperand->attrib.runtime.isAddress = true;
			attrib.runtime.hasDisplacement = false;
		}

		else if (encoded.modrm.rmBits == 0b100)
		{
			activeOperand->attrib.runtime.isAddress = true;
			attrib.runtime.hasDisplacement = false;
		}

		else if (encoded.modrm.rmBits == 0b101)
		{
			attrib.runtime.hasDisplacement = true;
		}
	}

	// Operand is located at a memory address found by combining a specified register and a one-byte displacement
	else if (encoded.modrm.modBits == 0b01)
	{
		activeOperand->attrib.runtime.isAddress = true;
		attrib.runtime.hasDisplacement = true;
		attrib.runtime.displacementSize = 1;
	}
	
	// Operand is located at a memory address found by combining a specified register and a four-byte displacement
	else if (encoded.modrm.modBits == 0b10)
	{
		activeOperand->attrib.runtime.isAddress = true;
		attrib.runtime.hasDisplacement = true;
		attrib.runtime.displacementSize = 4;
	}

	else if (encoded.modrm.modBits == 0b11)
	{
		activeOperand->attrib.runtime.isAddress = false;
		attrib.runtime.hasDisplacement = false;
		if (op1.attrib.intrinsic.size < 2 ) // 8 bit operand 
		{
			activeOperand->value = ModRMRegisterEncoding8.at(*operandField);
		}
		
		else // Default to 32-bit for now
		{
			activeOperand->value = ModRMRegisterEncoding32.at(*operandField);
		}
	}
}

void Instruction::InterpretSIBByte(byte sibByte)
// Hardcoded as op2 because in all instructions that use the SIB it will be used for op2 (?)
{
	// Bit-shifted for easier comparisons
	// Right bit shift on an unsigned integer results in a logical right shift
	encoded.sib.scaleBits = (sibByte & 0b11000000) >> 6;
	encoded.sib.indexBits = (sibByte & 0b00111000) >> 3;
	encoded.sib.baseBits  = (sibByte & 0b00000111);

	// Scale
	op2.attrib.runtime.sibScaleFactor = SIBScaleFactor.at(encoded.sib.scaleBits);

	// Index
	if (encoded.sib.indexBits == 0b100)
	{
		// Invalid index value, should not occur naturally
		// Consider changing this in the future to skip to the DecodingFailure state
		// once debugging is more robust
		throw std::logic_error("Encountered invalid index value of 0b100 when parsing SIB. This could be a false positive.");
	}
	
	else
	{
		op2.attrib.runtime.sibCompliment = SIBIndex.at(encoded.sib.indexBits);
	}

	// Base
	if (encoded.sib.baseBits == 0b101 && encoded.modrm.modBits == 0b00)
	// Displacement-only
	{
			
	}

	else if (encoded.sib.baseBits == 0b101 && (encoded.modrm.modBits == 0b10 && encoded.modrm.modBits == 0b01))
	// EBP
	{
		op2.value = OperandType::EBP;
	}

	else
	{
		op2.value = SIBBase.at(encoded.sib.baseBits);
	}
}

};
