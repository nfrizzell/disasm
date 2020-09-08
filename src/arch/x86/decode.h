#pragma once

#include <vector>
#include <map>

#include "../../util/common.h"

#include "instruction.h"

namespace ISet_x86
{

class LinearDecoder
{
public:
	LinearDecoder(std::vector<byte> * section);

	std::vector<Instruction> DecodeSection();

	Instruction::EncodedData::ModRM DissectModRMByte(byte modrm);
	Instruction::EncodedData::SIB DissectSIBByte(byte sibByte);

	byte CurrentByte();
	unsigned int ByteOffset();
	bool NextByte();
	void NextInstruction();
	void ChangeState(funcptr newState);

private:
	funcptr state {};
	Instruction currentInstr {};
	byte currentByte {};
	unsigned int byteOffset {}; // The index of the next byte to be read
	unsigned int stateLoopCounter {}; // Used to check for an infinite loop

	std::vector<byte> * section {}; // The current section (.text/.init/etc.) being parsed
	std::vector<Instruction> instructions {}; // Decoded instructions or data segments
};

class InstructionReference
{
public:
	bool Contains(Opcode opcode);
	bool ContainsPrimary(byte b);
	Instruction GetReference(Opcode opcode);
	void Emplace(Opcode op, Instruction instr);

private:
	// Loaded at runtime from the relevant .csv file
	std::map<Opcode, Instruction> reference;
};

extern InstructionReference instrReference;

};

namespace State_x86
{

using namespace ISet_x86;

// General states
void Init(LinearDecoder * context, Instruction &instr);
void Prefix(LinearDecoder * context, Instruction &instr);
void Opcode(LinearDecoder * context, Instruction &instr);
void Operands(LinearDecoder * context, Instruction &instr);
void DecodeSuccess(LinearDecoder * context, Instruction &instr);
void DecodeFailure(LinearDecoder * context, Instruction &instr);
void EndOfSegment(LinearDecoder * context, Instruction &instr);

// Addressing method states
void MethodError(LinearDecoder * context, Instruction &instr);

void MethodA(LinearDecoder * context, Instruction &instr);
void MethodB(LinearDecoder * context, Instruction &instr);
void MethodC(LinearDecoder * context, Instruction &instr);
void MethodD(LinearDecoder * context, Instruction &instr);
void MethodE(LinearDecoder * context, Instruction &instr);
void MethodF(LinearDecoder * context, Instruction &instr);
void MethodG(LinearDecoder * context, Instruction &instr);
void MethodH(LinearDecoder * context, Instruction &instr);
void MethodI(LinearDecoder * context, Instruction &instr);
void MethodJ(LinearDecoder * context, Instruction &instr);

void MethodL(LinearDecoder * context, Instruction &instr);
void MethodM(LinearDecoder * context, Instruction &instr);
void MethodN(LinearDecoder * context, Instruction &instr);
void MethodO(LinearDecoder * context, Instruction &instr);
void MethodP(LinearDecoder * context, Instruction &instr);
void MethodQ(LinearDecoder * context, Instruction &instr);
void MethodR(LinearDecoder * context, Instruction &instr);
void MethodS(LinearDecoder * context, Instruction &instr);

void MethodU(LinearDecoder * context, Instruction &instr);
void MethodV(LinearDecoder * context, Instruction &instr);
void MethodW(LinearDecoder * context, Instruction &instr);
void MethodX(LinearDecoder * context, Instruction &instr);
void MethodY(LinearDecoder * context, Instruction &instr);
void MethodZ(LinearDecoder * context, Instruction &instr);

void MethodRegister(LinearDecoder * context, Instruction &instr);

// Addressing method function mappings
const std::map<AddrMethod, funcptr> addrMethodHandler
{
	{ AddrMethod::NOT_APPLICABLE, MethodError },

	{ AddrMethod::A, MethodA },
	{ AddrMethod::B, MethodB },
	{ AddrMethod::C, MethodC },
	{ AddrMethod::D, MethodD },
	{ AddrMethod::E, MethodE },
	{ AddrMethod::F, MethodF },
	{ AddrMethod::G, MethodG },
	{ AddrMethod::H, MethodH },
	{ AddrMethod::I, MethodI },
	{ AddrMethod::J, MethodJ },

	{ AddrMethod::L, MethodL },
	{ AddrMethod::M, MethodM },
	{ AddrMethod::N, MethodN },
	{ AddrMethod::O, MethodO },
	{ AddrMethod::P, MethodP },
	{ AddrMethod::Q, MethodQ },
	{ AddrMethod::R, MethodR },
	{ AddrMethod::S, MethodS },

	{ AddrMethod::U, MethodU },
	{ AddrMethod::V, MethodV },
	{ AddrMethod::W, MethodW },
	{ AddrMethod::X, MethodX },
	{ AddrMethod::Y, MethodY },

	{ AddrMethod::HARDCODED_REGISTER, MethodRegister }
};

};
