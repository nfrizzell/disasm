#pragma once

#include <vector>
#include <array>
#include <map>

#include "../../util/common.h"
#include "../../util/safebool.h"

namespace ISet_x86
{

class LinearDecoder;
class Instruction;

using funcptr = void (*)(LinearDecoder * context, Instruction &instr);

const std::vector<byte> prefixes
{
	// Group 1
	0xF0,		// Lock
	0xF2,		// REPNE/REPNZ
	0xF3,		// Rep/REPE/REPZ

	// Group 2	// Segment overrides
	0x2E,		// CS / branch not taken
	0x36,		// SS
	0x3E,		// DS / branch taken
	0x26,		// ES
	0x64,		// FS
	0x65,		// GS

	// Group 3	
	0x66,		// Operand-size modifier

	// Group 4
	0x67		// Address-size modifier
};

enum class AddrMethod : int8_t
// These are the operand addressing methods as outlined in section A.2.1 of the 
// Intel x86 software developer's manual
// Commented-out values are those that are in the documentation but not used here
{
	NOT_APPLICABLE = INVALID,
	A = 0, // Direct address, address of operand is encoded in instruction	
	B = 1, // General-purpose register encoded in the vvvv field of the VEX prefix
	C = 2, // Control register encoded in the reg field of the ModR/M byte
	D = 3, // Debug register encoded in the reg field of the ModR/M byte

	E = 4, // ModR/M follows opcode and specifies operand, which can be either
	       // a general-purpose register or a memory address. If it is a memory
	       // address, the address is computed from a segment register and:
	       // 	- a base register
	       // 	- an index register
	       // 	- a scaling factor
	       // 	- a displacement

	F = 5, // The operand is located in the EFLAGS/RFLAGS register
	G = 6, // Reg field of the ModR/M byte encodes a general-purpose register
	H = 7,
	I = 8, // Immediate data
	J = 9, // The instruction encodes a relative offset to add to the instruction
	       // pointer register
	L =10, // The upper 4 bits of the 8 bit immediate encode an XMM or YMM register
	M =11, // The ModR/M byte may refer only to memory
	N =12, // The R/M field of the ModR/M byte encodes a packed-quadword MMX register
	O =13, // No ModR/M byte, the offset of the operand is encoded as a word/dword in 			 // the instruction
	P =14, // The reg field of the ModR/M byte encodes a packed quadword MMX register

	Q =15, // The ModR/M byte specifies the operand, which is either an MMX register			 // or a memory address. If it is a memory address, the address is computed
	       // from a segment register and:
	       //       - a base register
	       //       - an index register
	       //       - a scaling factor
	       //       - a displacement

	R =16, // The R/M field of the ModR/M byte encodes a general-purpise register
	S =17, // The reg field of the ModR/M byte encodes a segment register

	U =18, // The R/M field of the ModR/M byte encodes a XMM register or YMM register
	V =19, // The reg field of the ModR/M byte encodes a XMM register or YMM register

	W =20, // The ModR/M byte specifies the operand, which is either a XMM register,
	       // YMM register, or a memory address. If it is a memory address, the
	       // address is computed from a segment register and:
	       //       - a base register
	       //       - an index register
	       //       - a scaling factor
	       //       - a displacement
	
	X =21, // Memory addressed by the DS and rSI register pair
	Y =22, // Memory addressed by the ES and rDI register pair

	// Extended operand addressing methods
	Z =23,
	BA=24,
	BB=25,
	BD=26,
	ES=27,
	EST=28,
	SC=29,
	
	HARDCODED_REGISTER = 100,
	HARDCODED_VALUE = 101,
	X87_STACK = 102
};

// The range that register types fall into in the enum
const int REGISTER_LOWER_BOUND = 100;
const int REGISTER_UPPER_BOUND = 2000;

enum class OperandType : int16_t
{
	NOT_APPLICABLE = INVALID,
	a = 0, // Two one-word operands or double-word operands in memory, depending on
	       // operand-size attribute
	b = 1, // Byte
	c = 2, // Byte or word, depending on operand-size attribute
	d = 3, // Doubleword
	dq= 4, // Double-quadword
	p = 5, // 32-bit, 48-bit, or 80-bit pointer, depending on operand-size attribute
	pd= 6, // 128-bit or 256-bit packed double-precision floating-point data
	pi= 7, // Quadword MMX register
	ps= 8, // 128-bit or 256-bit packed single-precision floating-point data
	q = 9, // Quadword
	qq=10, // Quad-quadword
	s =11, // 6-byte psuedo-descriptor
	sd=12, // Scalar element of a 128-bit double-precision floating data
	ss=13, // Scalar element of a 128-bit single-precision floating data
	si=14, // Doubleword integer register (e.g. EAX)
	v =15, // Word, doubleword, or quadword, depending on operand-size attribute
	w =16, // Word
	x =17, // Double-quadword or quad-quadword, depending on operand-size attribute
	y =18, // Doubleword or quadword, depending on operand-size attribute
	z =19, // Word for 16-bit operand-size or doubleword for 32/64-bit operand-size

	// Extended operand types
	bcd=20,
	bs=21,
	bsq=22,
	bss=23,
	di=24,
	dqp=25,
	dr=26,
	e=27,
	er=28,
	psq=29,
	ptp=30,
	qi=31,
	sr=32,
	st=33,
	stx=34,
	vds=35,
	vqp=36,
	vs=37,
	wi=38,
	va=39,
	dqa=40,
	wa=41,
	wo=42,
	ws=43,
	da=44,
	do_=45,
	qa=46,
	qs=47,

	// General-purpose registers
	AH = 101, AL = 102, AX = 103, EAX = 104,
	BH = 201, BL = 202, BX = 203, EBX = 204,
	CH = 301, CL = 302, CX = 303, ECX = 304,
	DH = 401, DL = 402, DX = 403, EDX = 404,
	// Pointer registers
	SI = 501, ESI = 502,
	DI = 601, EDI = 602,
	BP = 701, EBP = 702,
	SP = 801, ESP = 802,
	// Segment registers
	SS = 901, CS = 902, DS = 903, ES = 904, FS = 905, GS = 906,
	// Instruction pointer
	IP = 1001, EIP = 1002,
	// EFLAGS register
	EFLAGS = 1101,
	
	// FPU/x87 stack
	X87_STACK_TOP = 1201,
	X87_STACK_1 = 1202,
	X87_STACK_2 = 1203,
	X87_STACK_3 = 1204, 
	X87_STACK_4 = 1205, 
	X87_STACK_5 = 1206, 
	X87_STACK_6 = 1207,
	X87_STACK_7 = 1208,

	// Other special types
	IMMD = 2001, // Immediate value
	HARDCODED_VALUE = 2101 //  For instructions that have intrinsic constant operands
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

	int8_t weight {}; // Tiebreaker when all other attributes are equal

	bool operator==(const Opcode &rhs) const;
	bool operator<(const Opcode &rhs) const;
};

class Operand
// Operand value will be of one of two types: a numeric value (int) or a register enum
// value. Regardless of the type of the operand value, it will always be returned as int.
{
public:
	struct OperandAttributes
	{
		struct Intrinsic
		{
			AddrMethod addrMethod {AddrMethod::NOT_APPLICABLE}; 
			OperandType type {OperandType::NOT_APPLICABLE}; 
			uint8_t size {}; // Size of operand in bytes

		} intrinsic {};

		struct Runtime
		{
			bool isAddress {}; // If an operand is a memory address
			SafeBool isRegister {}; // If operand is a hardcoded/encoded register
			uint8_t opcodeLength {};

			// Applicable to SIB mode only
			uint8_t sibScaleFactor {}; // Size of optional SIB scale factor (1/2/4/8)
			OperandType sibCompliment {OperandType::NOT_APPLICABLE}; // SIB index reg

		} runtime {};

	} attrib {};

	// The type of the value the operand holds and not the type of the operand
	OperandType value {OperandType::NOT_APPLICABLE};
	
	void UpdateSize(std::array<byte, 4> &prefixes);
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
			int8_t operationMode {INVALID};
			int8_t ringLevel {INVALID};
			int8_t lockAndFPU {INVALID};
			std::string iext {"UNRESOLVED"};
			std::string group1 {"UNRESOLVED"};
			std::string group2 {"UNRESOLVED"};
			std::string group3 {"UNRESOLVED"};

			int16_t testedFlags {INVALID};
			int16_t modifiedFlags {INVALID};
			int16_t definedFlags {INVALID};
			int16_t undefinedFlags {INVALID};
			int16_t flagValues {INVALID};

			std::string description {"UNRESOLVED"};

		} intrinsic {};

		struct Runtime
		{
			uint8_t size {}; // Total byte size of the entire instruction
			uint64_t segmentByteOffset {}; // The index to the start of the instr
			uint8_t prefixCount {}; // Number of prefixes attached to the instruction
			uint8_t opcodeLength {}; // Not including mandatory prefix
			uint8_t displacementSize {}; // Size of displacement in bytes  

			SafeBool resolved {}; // If the instruction has been successfully read
			bool hasSIB {};
			bool hasDisplacement {};
			bool op1Read {}; // Check if op1 has been read before op2
			bool op2Read {}; // Check if op1 has been read before op2
			bool modRMRead {};
			bool sibRead {};

		} runtime {};

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

		int disp {}; 	  // Displacement constant used by certain addressing modes
		std::array<byte, 4> immd {}; // Constant operand encoded after instruction

	} encoded {};

	Operand op1 {}; // (Generally) written to
	Operand op2 {}; // (Generally) read from
	Operand op3 {}; // Used by some instructions
	Operand op4 {}; // Used by some instructions
	Operand * activeOperand {}; // Pointer to the active operand being read
	
	void InterpretModRMByte(const byte modrmByte);
	void InterpretSIBByte(const byte sibByte);

	void UpdateAttributes(const Instruction &reference);

private:
	// Gives either a 16-bit or 32-bit register based on the presence of the 0x67 prefix
	OperandType ModRMRegValue(int rmBits);
	uint8_t OperandByteSize(); // Returns size of operand in bytes based on prefix & type
};

};
