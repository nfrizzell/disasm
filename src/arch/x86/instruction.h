#pragma once

#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <iostream>

#include "../../util/common.h"
#include "reference.h"

namespace ISet_x86
{

class LinearDecoder;
class Instruction;

using funcptr = void (*)(LinearDecoder * context, Instruction &instr);

// Used for checking whether or not a given pair of primary opcode and twobyte flag
// signals that a secondary opcode will be present
struct ThreeByteKey
{
	bool twoByte {};
	int16_t primary {INVALID};
	bool operator==(const ThreeByteKey &tbk) const;
};

struct ThreeByteHash 
{
	std::size_t operator()(const ThreeByteKey &op) const;
};

struct Opcode
{
	int16_t mandatoryPrefix {INVALID};
	bool twoByte {};
	int16_t primary {INVALID};
	int16_t secondary {INVALID};
	int8_t extension {INVALID};

	struct OpcodeFields
	{
		bool regEncoded {};
		bool operandSize {};
		bool signExtend {};
		bool direction {}; // False/default indicates normal, aka REG source operand
		int8_t conditionals {INVALID}; // four bits, valid with values 0 - 15
		// "sr" and "sre" are represented by this one value
		int8_t segmentRegisterSpecifier {INVALID}; // three bits, valid with values 0 - 7
		int8_t memoryFormat {INVALID}; // two bits, valid with values 0 - 3

	} fields {};

	friend std::ostream & operator<<(std::ostream &out, const Opcode &opcode);
	bool operator==(const Opcode &rhs) const;
};

struct OpcodeHash
{
	std::size_t operator()(const Opcode &op) const;
};

class Operand
{
public:
    //
    enum Encoding
    {
        NOT_APPLICABLE = INVALID,
		IMMD = 0,
		RELATIVE_DISPLACEMENT,
		OPCODE_REGISTER,
		MODRM_REGISTER_REGBITS,
		MODRM_REGISTER_RMBITS, // When the REG and RM fields of the ModRM byte are flipped
		MODRM_REGISTER_WITH_DISP,
		MODRM_REGISTER_SCALED,
		MODRM_REGISTER_SCALED_WITH_DISP
    };

	struct OperandAttributes
	{
		struct Intrinsic
		{
			AddrMethod addrMethod {AddrMethod::NOT_APPLICABLE}; 
			OperandType type {OperandType::NOT_APPLICABLE}; 

		} intrinsic {};

        struct Runtime
        {
            Encoding encoding {Encoding::NOT_APPLICABLE};
		} runtime {};

	} attrib {};
};

class Instruction
{
public:
	struct InstructionAttributes
	{
		struct Intrinsic
		{
			std::string mnemonic {"UNRESOLVED"};
			// First processor the instruction was supported by 
			std::string firstAppearance {"UNRESOLVED"};
			std::string documentationStatus {"UNRESOLVED"};
			char operationMode {INVALID};
			char ringLevel {INVALID};
			bool plusr {};
			bool lock {};
			bool fpush {};		
			bool fpop {};		
			std::string alias {"UNRESOLVED"};
			std::string iext {"UNRESOLVED"};
			std::string group1 {"UNRESOLVED"};
			std::string group2 {"UNRESOLVED"};
			std::string group3 {"UNRESOLVED"};

			std::string testedFlags {"UNRESOLVED"};
			std::string modifiedFlags {"UNRESOLVED"};
			std::string definedFlags {"UNRESOLVED"};
			std::string undefinedFlags {"UNRESOLVED"};
			std::string flagValues {"UNRESOLVED"};

			std::string brief {"UNRESOLVED"};

            bool x86Exclusive {};
            bool x64Exclusive {};
		} intrinsic {};

		struct Runtime
		{
			uint8_t size {}; // Total byte size of the entire instruction
			uint64_t segmentByteOffset {}; // The index to the start of the instr
			uint8_t prefixCount {}; // Number of prefixes attached to the instruction
			uint8_t opcodeLength {}; // Not including mandatory prefix
			uint8_t displacementSize {}; // Size of displacement in bytes  

		} runtime {};

		struct Flags
		{
			bool hasSIB {};
			bool hasDisplacement {};

			bool modRMRead {};
			bool sibRead {};
			bool dispRead {};

            bool op1Read {};
            bool op2Read {};
            bool op3Read {};
            bool op4Read {};

            bool resolved {}; // If the instruction has been successfully read

		} flags {};

	} attrib {};

	struct EncodedData
	// The actual encoded byte data of each instruction
	{
		std::array<byte, 4> prefix {};
		Opcode opcode {};

		struct ModRM
		{
			uint8_t modBits {};   // 11000000
			uint8_t regOpBits {}; // 00111000
			uint8_t rmBits {};    // 00000111

		} modrm {};

		struct SIB
		{
			uint8_t scaleBits {}; // 11000000
			uint8_t indexBits {}; // 00111000
			uint8_t baseBits {};  // 00000111

		} sib {};

		unsigned int disp {}; 	  // Displacement constant used by certain addressing modes
		unsigned int immd {}; // Constant operand encoded after instruction
    } encoded {};

    Operand op1 {};
    Operand op2 {};
    Operand op3 {};
    Operand op4 {};

	Operand * activeOperand;
	
	void InterpretModRMByte(const byte modrmByte);
	void InterpretSIBByte(const byte sibByte);

	void UpdateAttributes(const Instruction &reference);

	friend std::ostream & operator<<(std::ostream &out, const Instruction &instr);

private:
	// Gives either a 16-bit or 32-bit register based on the presence of the 0x67 prefix
	OperandType ModRMRegValue(int rmBits);
	uint8_t OperandByteSize(); // Returns size of operand in bytes based on prefix & type
};

};
